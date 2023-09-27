from decimal import Decimal
from django.shortcuts import render
from django.http import HttpResponse, Http404

from django.shortcuts import redirect, get_object_or_404
from django.urls import reverse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout

def getHome(request):
     
     context = {}

     if request.method == 'GET':
        context['form'] = ''
        context['output'] = ''
        return render(request, 'home.html', context)