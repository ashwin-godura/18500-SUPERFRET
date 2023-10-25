from decimal import Decimal
import time
from django.conf import settings
from django.shortcuts import render
from django.http import HttpResponse, Http404

from django.shortcuts import redirect, get_object_or_404
from django.urls import reverse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from app.forms import AddFileForm
from app.models import MidiFile
import os

# from app.uart import Uart

# u = Uart()

def getHome(request):  
   context = {}

   MidiFiles = MidiFile.objects.all()

   file = findactivefile()

   if not file is None:
      context['activefile'] = True
   else:
      context['activefile'] = False

   context['MidiFileForm'] = AddFileForm()
   context['ScaleMidiFiles'] = MidiFiles.filter(type="scale")
   context['SongMidiFiles'] = MidiFiles.filter(type="song")
   return render(request, 'home.html', context)

def addFile(request):  
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
      return getHome(request)
   
   file = MidiFile.objects.all().filter(name=name)[0]
   file.active = True
   file.save()
   # u.start_song(str(file.file))

   return getHome(request)

def deletefile(request, name):
   file = findactivefile()
   if file is not None:
      return getHome(request)
   
   file = MidiFile.objects.all().filter(name=name)[0]
   os.remove(str(file.file))
   file.delete()
   return getHome(request)


def stopfile(request):
   file = findactivefile()
   file.active = False
   file.save()
   # u.stop_song()
      
   return getHome(request)


def findactivefile():
   files = MidiFile.objects.all()
   for file in files:
      if file.active:
         return file
   return None

def movingblock(request):
    return render(request, 'movingblock.html')