from django.db import models
from django.contrib.auth.models import User


class MidiFile(models.Model):
    name = models.CharField(max_length=255)  # To store the file name
    file = models.FileField(upload_to='midi_files/')  # To store the file itself
    class Meta:
        app_label = 'app'

class StaticMidiFile(models.Model):
    name = models.CharField(max_length=255)  # To store the file name
    file = models.FileField(upload_to='midi_files/')  # To store the file itself
    class Meta:
        app_label = 'app'
