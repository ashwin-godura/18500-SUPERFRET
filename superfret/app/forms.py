from django import forms
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from django.core.validators import MaxValueValidator, MinValueValidator
from django.core.exceptions import ValidationError
import os

FILE_TYPE=(
    ('song', 'song'),
    ('scale', 'scale'),
)

class AddFileForm(forms.Form):
    name = forms.CharField(max_length=255)
    type = forms.ChoiceField(choices=FILE_TYPE)
    file = forms.FileField(
        label='Select static file',
        widget=forms.ClearableFileInput(attrs={'class': 'form-control-file'})
    )

