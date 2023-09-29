from django import forms
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from django.core.validators import MaxValueValidator, MinValueValidator
from django.core.exceptions import ValidationError
import os



class AddStaticFileForm(forms.Form):
    name = forms.CharField(max_length=255)
    file = forms.FileField(
        label='Select static file',
        widget=forms.ClearableFileInput(attrs={'class': 'form-control-file'})
    )

