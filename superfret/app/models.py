from django.db import models
from django.contrib.auth.models import User


class StaticMidiFile(models.Model):
    name = models.CharField(max_length=255)  # To store the file name
    file = models.FileField()  # To store the file itself
    active = models.BooleanField(default=False)

    def __unicode__(self): 
        return self.name
