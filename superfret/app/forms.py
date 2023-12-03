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
    def clean_file(self):
        cleaned_data = super(AddFileForm, self).clean()
        file = cleaned_data.get('file')

        if file:
            # Check file type
            file_name = file.name.lower()
            if not file_name.endswith(('.mid', '.midi')):
                raise forms.ValidationError('Invalid file type. Please upload a .mid or .midi file.')

            # Check file size
            max_size_kb = 100
            max_size_bytes = max_size_kb * 1024
            if file.size > max_size_bytes:
                raise forms.ValidationError('File size must be no more than 100 KB.')

        return file

