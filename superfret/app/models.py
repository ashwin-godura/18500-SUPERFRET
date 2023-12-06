from django.db import models
from django.contrib.auth.models import User

FILE_TYPE=(
    ('song', 'song'),
    ('scale', 'scale'),
)


class MidiFile(models.Model):
    name = models.CharField(max_length=255)  # To store the file name
    file = models.FileField()  # To store the file itself
    active = models.BooleanField(default=False)
    type = models.CharField(max_length=255)

    def __unicode__(self): 
        return self.name
    
class StrumEvent(models.Model):
    midi_file = models.ForeignKey(MidiFile, on_delete=models.CASCADE, related_name='strum_events')
    correct = models.BooleanField()
    timing = models.FloatField()

    def __unicode__(self):
        return f"StrumEvent for {self.midi_file.name}"
