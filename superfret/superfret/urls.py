"""superfret URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from app import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('home/', views.getHome, name='home'),
    path('', views.getHome, ),
    path('addFile', views.addFile, name='addFile'),
    path('startfile/<str:name>/', views.startfile, name='startfile'),
    path('deletefile/<str:name>/', views.deletefile, name='deletefile'),
    path('stopfile', views.stopfile, name='stopfile'),
    path('pausefile', views.pausefile, name='pausefile'),
    path('restartfile', views.restartfile, name='restartfile'),
    path('playingFile', views.playingFile, name='playingFile'),
    path('getActiveFile', views.getActiveFile, name='getActiveFile'),
    path('shutDownNow', views.shutDownNow, name='shutDownNow'),
    path('getFeedback', views.getFeedback, name='getFeedback'),
    path('viewFeedback/<str:name>/', views.viewFeedback, name='viewFeedback'),
    path('getBassGuitarImage/', views.getBassGuitarImage, name='getBassGuitarImage')
]
