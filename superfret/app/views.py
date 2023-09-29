from decimal import Decimal
from django.shortcuts import render
from django.http import HttpResponse, Http404

from django.shortcuts import redirect, get_object_or_404
from django.urls import reverse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from app.forms import AddStaticFileForm
from app.models import StaticMidiFile

def getHome(request):  
   context = {}

   staticMidiFiles = StaticMidiFile.objects.all()

   file = findactivefile()

   if not file is None:
      context['activefile'] = True
   else:
      context['activefile'] = False

   context['StaticMidiFileForm'] = AddStaticFileForm()
   context['StaticMidiFiles'] = staticMidiFiles
   return render(request, 'home.html', context)

def addStatic(request):  
   if request.method == 'GET':
      return redirect(reverse('home'))
     
   form = AddStaticFileForm(request.POST, request.FILES)
   
   print(str(form))

   file = StaticMidiFile()
   file.name = form.cleaned_data['name']
   file.file = form.cleaned_data['file']
   file.save()
   print("saved the file")
   return redirect(reverse('home'))

def startfile(request, name):
   file = StaticMidiFile.objects.all().filter(name=name)[0]
   file.active = True
   file.save()

   return getHome(request)

def stopfile(request):
   file = findactivefile()
   file.active = False
   file.save()
      
   return getHome(request)



def findactivefile():
   files = StaticMidiFile.objects.all()
   for file in files:
      if file.active:
         return file
   return None
     
   # search regular midi files too  

   # files = MidiFile.objects.all()
   # for file in files:
   #    if file.active:
   #       file.active = False