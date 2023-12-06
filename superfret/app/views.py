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
from app.models import MidiFile, StrumEvent
import os

import matplotlib.pyplot as plt
from io import BytesIO
import base64

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
   
   # Assuming 'midi_file_instance' is an instance of MidiFile
   active.strum_events.all().delete()


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
   active = findactivefile()

   if active is None:
      return JsonResponse()

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
      strum_event = StrumEvent.objects.create(
         midi_file=active,
         correct=feedback['correct'],  # Set your desired boolean value
         timing=feedback['diff'],  # Set your desired float value for timing
      )
      strum_event.save()


   return JsonResponse(data)



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
   
def viewFeedback(request, name):
    # Get the MidiFile instance by name
    midi_file_instance = get_object_or_404(MidiFile, name=name)

    # Get all StrumEvent instances related to the MidiFile
    strum_events = midi_file_instance.strum_events.all()

    if strum_events is None or len(strum_events) == 0:
       return redirect(reverse('home'))

    # Plot 1: Timing vs StrumEvent ID for Correct StrumEvents
    correct_events = [event for event in strum_events if event.correct]
    plt.figure(figsize=(8, 6))
    plt.scatter([event.id for event in correct_events], [event.timing for event in correct_events])
    plt.xlabel('StrumEvent ID')
    plt.ylabel('Timing')
    plt.title('Timing vs StrumEvent ID for Correct StrumEvents')
    plt.grid(True)
    timing_plot_img = get_image_as_base64(plt)
    plt.close()

    # Plot 2: Total Strums vs StrumEvent ID for Correct StrumEvents
    plt.figure(figsize=(8, 6))
    total_strums = 0
    y_values = []
    for event in strum_events:
        total_strums += 1
        if event.correct:
            total_strums = 0
            y_values.append(total_strums)

    x_values = range(0, len(y_values))
    plt.plot(x_values, y_values, marker='o')
    plt.xlabel('StrumEvent ID')
    plt.ylabel('Total Strums to Correct')
    plt.title('Total Strums vs StrumEvent ID for Correct StrumEvents')
    plt.grid(True)
    total_strums_plot_img = get_image_as_base64(plt)
    plt.close()

    # Render the HTML response
    context = {
        'timing_plot': timing_plot_img,
        'total_strums_plot': total_strums_plot_img,
    }
    return render(request, 'feedback.html', context)


def get_image_as_base64(plt):
    # Convert the Matplotlib figure to base64 for embedding in HTML
    buffer = BytesIO()
    plt.savefig(buffer, format='png')
    buffer.seek(0)
    image_base64 = base64.b64encode(buffer.read()).decode('utf-8')
    return f"data:image/png;base64,{image_base64}"