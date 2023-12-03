from decimal import Decimal
import threading
import time
from django.conf import settings
from django.shortcuts import render
from django.http import HttpResponse, Http404

from django.shortcuts import redirect, get_object_or_404
from django.urls import reverse
from django.http import JsonResponse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from app.forms import AddFileForm
from app.models import MidiFile
import os

from app.uart import Uart
import app.MidiFileReader

u = Uart()

def getHome(request):  
   context = {}

   MidiFiles = MidiFile.objects.all()

   file = findactivefile()

   if not file is None:
      return redirect(reverse('playingFile'))

   context['MidiFileForm'] = AddFileForm()
   context['ScaleMidiFiles'] = MidiFiles.filter(type="scale")
   context['SongMidiFiles'] = MidiFiles.filter(type="song")
   return render(request, 'home.html', context)

def addFile(request):  
   context = {}
   if request.method == 'GET':
      return redirect(reverse('home'))
     
   form = AddFileForm(request.POST, request.FILES)
   
   # for some reason this print statement is integal to the functioning of this website
   # print(str(form))
   if not form.is_valid():
      return redirect(reverse('home'))

   file = MidiFile()
   file.name = form.cleaned_data['name']
   file.file = form.cleaned_data['file']
   file.type = form.cleaned_data['type']
   file.save()
   return redirect(reverse('home'))

def startfile(request, name):
   file = findactivefile()
   if file is not None:
      return redirect(reverse('home'))
   
   file = MidiFile.objects.all().filter(name=name)[0]
   file.active = True
   file.save()

   return redirect(reverse('playingFile'))

def deletefile(request, name):
   file = findactivefile()
   if file is not None:
      return redirect(reverse('home'))
   
   file = MidiFile.objects.all().filter(name=name)[0]
   os.remove("app/static/images/" + str(file.file))
   file.delete()
   return redirect(reverse('home'))


def stopfile(request):
   file = findactivefile()
   if file is not None:
      file.active = False
      file.save()
   u.restart_song()
      
   return redirect(reverse('home'))

def pausefile(request):

   u.pause_song()
      
   return redirect(reverse('home'))

def restartfile(request):
   u.restart_song()
      
   return redirect(reverse('home'))


def findactivefile():
   files = MidiFile.objects.all()
   for file in files:
      if file.active:
         return file
   return None

def playingFile(request):
   active = findactivefile()
   if active is None:
      return redirect(reverse('home'))
   
   tracks = app.MidiFileReader.get_midi_tracks("app/static/images/" + str(active.file))
   # print(tracks)
   context = {
      "file": active,
      "tracks": tracks
   }
   return render(request, 'playingFile.html', context)

def getActiveFile(request):
   speed = int(request.GET.get('speed', 1))
   transpose = int(request.GET.get('transpose', 0))
   track = str(request.GET.get('track', ""))
   mode = str(request.GET.get('mode', ""))
   metronome = int(request.GET.get('metronome', 0))


   active = findactivefile()

   if active is None:
      return redirect(reverse('home'))

   file_path = "app/static/images/" + str(active.file)


   notes_for_webapp, tempo = app.MidiFileReader.extract_notes_from_midi(file_path, speed, transpose, track) 
   notes_for_teensy = app.MidiFileReader.convert_notes_to_bytes(notes_for_webapp, tempo, mode, metronome)

   u.restart_song()
   u.start_song(notes_for_teensy)
   data = {
      'notes': notes_for_webapp,
   }
   
   threading.Thread(target=getFeedback).start

   return JsonResponse(data)

def getFeedback(request):
   feedbacks = u.read_feedback()

   if feedbacks is None:
      data = {
         'valid': False,
      }
   else:
      feedback = feedbacks[-1]
      data = {
         'playTime': feedback['diff'],
         'correct': feedback['correct'],
         'valid': True,
      }

   return JsonResponse(data)

# def getFeedback(request):
#    print("starting get feedback")
#    while True:
#       layTime, correct = u.read_feedback() or (None, None)



def shutDownNow(request):
    # Render the shutdown.html template
    response = render(request, 'shutdown.html')

    # Start a separate thread to execute the shutdown command after a delay
    shutdown_thread = threading.Thread(target=delayed_shutdown)
    shutdown_thread.start()

    # Return the response to the client
    return response

def delayed_shutdown():
   # Wait for one second
   time.sleep(1)

   # Define the command to be executed
   command = "shutdown now"

   # Use subprocess.run with sudo and shell=True
   os.system(command)

def getBassGuitarImage(request):
   # Assuming your images are stored in a 'images' directory within your media root
   image_path = "app/static/images/All-notes-on-bass-horiz.webp"

   # Check if the image exists
   with open(image_path, 'rb') as image_file:
      response = HttpResponse(image_file.read(), content_type='image/jpeg')
      return response