from decimal import Decimal
from django.shortcuts import render
from django.http import HttpResponse, Http404

from django.shortcuts import redirect, get_object_or_404
from django.urls import reverse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from app.forms import AddDynamicFileForm, AddStaticFileForm
from app.models import MidiFile, StaticMidiFile

def getHome(request):  
     context = {}

     staticMidiFiles = StaticMidiFile.objects.all()
     midiFiles = MidiFile.objects.all()

     print(staticMidiFiles)

     if request.method == 'GET':
        context['StaticMidiFileForm'] = AddStaticFileForm()
        context['MidiFileForm'] = AddDynamicFileForm()
        context['MidiFiles'] = midiFiles
        context['StaticMidiFiles'] = staticMidiFiles
        return render(request, 'home.html', context)

def addStatic(request):  
   if request.method == 'GET':
      return redirect(reverse('home'))
     
   form = AddStaticFileForm(request.POST)

   if not form.is_valid():
       return redirect(reverse('home'))

   file = StaticMidiFile()
   file.name = form.name
   file.file = form.file
   file.save()
   return redirect(reverse('home'))

def addDynamic(request):  
   if request.method == 'GET':
      return redirect(reverse('home'))
     
   form = AddDynamicFileForm(request.POST)

   if not form.is_valid():
       return redirect(reverse('home'))

   file = MidiFile()
   file.name = form.name
   file.file = form.file
   file.save()
   return redirect(reverse('home'))


   


         