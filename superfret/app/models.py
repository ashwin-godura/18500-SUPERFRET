from django.db import models
from django.contrib.auth.models import User



class Profile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    isChild = models.BooleanField(default=False)
    ChildProfile = models.ForeignKey(User, on_delete=models.CASCADE, related_name='child', null=True)
    ParentProfile = models.ForeignKey(User, on_delete=models.CASCADE, related_name='parent', null = True)
    balance = models.DecimalField(decimal_places=2, max_digits=15, default=0)
    savings = models.DecimalField(decimal_places=2, max_digits=15, default=0)
    rate = models.DecimalField(decimal_places=2, max_digits=15, default=0)


    def __unicode__(self): 
        return self.name

''' 
class ParentInfo(models.Model):
    FirstName = models.CharField(max_length=250)
    LastName = models.CharField(max_length=350)
    balance = models.FloatField()

    def __unicode__(self): 
        return self.name

class ChildInfo(models.Model):
    pid = models.ForeignKey(ParentInfo, on_delete=models.CASCADE)
    FirstName = models.CharField(max_length=250)
    LastName = models.CharField(max_length=350)
    EmailLogin = models.CharField(max_length=300)
    Balance = models.FloatField()

    def __unicode__(self): 
        return self.name
  '''      

class Goals(models.Model):
    cid = models.ForeignKey(Profile, on_delete=models.CASCADE,related_name='childGoals')
    goal = models.CharField(max_length=300)
    cost = models.DecimalField(decimal_places=2, max_digits=15, default=0)
    complete = models.BooleanField()

    def __unicode__(self): 
        return self.name

class Chores(models.Model):
    cid = models.ForeignKey(Profile, on_delete=models.CASCADE, related_name='childChores')
    chore = models.CharField(max_length=300)
    complete = models.BooleanField()
    reward = models.DecimalField(decimal_places=2, max_digits=15, default=0)

    def __unicode__(self): 
        return self.name







# Create your models here.

