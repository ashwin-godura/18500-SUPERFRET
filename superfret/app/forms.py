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
        label='Select file',
        widget=forms.ClearableFileInput(attrs={'class': 'form-control-file'})
    )
    # Customizes form validation for properties that apply to more
    # than one field.  Overrides the forms.Form.clean function.
    def clean(self):
        # Calls our parent (forms.Form) .clean function, gets a dictionary
        # of cleaned data as a result
        cleaned_data = super().clean()
        return cleaned_data

