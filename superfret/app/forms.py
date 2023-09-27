from django import forms
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from django.core.validators import MaxValueValidator, MinValueValidator
from django.core.exceptions import ValidationError
import os

# Custom validator to check file extension
def validate_midi_extension(value):
    ext = os.path.splitext(value.name)[1].lower()
    if ext not in ['.midi', '.mid']:
        raise ValidationError('Only .midi or .mid files are allowed.')

class AddStaticFileForm(forms.Form):
    name = forms.CharField(max_length=255)
    file = forms.FileField(
        label='Select a .midi or .mid file',
        help_text='Max size: 5 MB',
        validators=[validate_midi_extension],
        widget=forms.ClearableFileInput(attrs={'class': 'form-control-file'})
    )
    name = forms.CharField(max_length=255)

class AddDynamicFileForm(forms.Form):
    name = forms.CharField(max_length=255)
    file = forms.FileField(
        label='Select a .midi or .mid file',
        help_text='Max size: 5 MB',
        validators=[validate_midi_extension],
        widget=forms.ClearableFileInput(attrs={'class': 'form-control-file'})
    )
    name = forms.CharField(max_length=255)


