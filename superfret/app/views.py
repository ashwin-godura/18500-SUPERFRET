from decimal import Decimal
from django.shortcuts import render
from django.http import HttpResponse, Http404
from app.forms import LoginForm
from app.forms import RegisterForm
from app.forms import addChoreForm
from app.forms import addGoalForm
from app.forms import createChildForm

from django.shortcuts import render, redirect, get_object_or_404
from django.urls import reverse

from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from app.models import Profile, Chores, Goals

# Create your views here.
def index_action(request):
    context = {}

    if request.method == 'GET':
        return render(request, 'index.html', context)

def login_action(request):
    context = {}

    if request.method == 'GET':
        context['form'] = LoginForm()
        context['output'] = ''
        return render(request, 'login.html', context)

    form = LoginForm(request.POST)
    context['form'] = form

    # Validates the form.
    if not form.is_valid():
        return render(request, 'login.html', context)

    new_user = authenticate(username=form.cleaned_data['username'],
                            password=form.cleaned_data['password'])

    login(request, new_user)
    return redirect(reverse('global'))

def parent_action(request):
    context = {}

    if request.method == 'GET':
        return render(request, 'parent.html', context)

def child_action(request):
    context = {}


    if request.method == 'GET':
        return render(request, 'child.html', context)

def register_action(request): 
    context = {}

    if request.method == 'GET':
        context['form'] = RegisterForm()
        context['output'] = ''
        return render(request, 'register.html', context)

    # Creates a bound form from the request POST parameters and makes the 
    # form available in the request context dictionary.
    form = RegisterForm(request.POST)
    context['form'] = form

    # Validates the form.
    if not form.is_valid():
        return render(request, 'register.html', context)

    # checks if the passwords are the same

    # if form.cleaned_data['password'] != form.cleaned_data['confirm_password']:
    #     return render(request, 'register.html', context)

    # At this point, the form data is valid.  Register and login the user.
    new_user = User.objects.create_user(username=form.cleaned_data['username'], 
                                        password=form.cleaned_data['password'],
                                        email=form.cleaned_data['email'],
                                        first_name=form.cleaned_data['first_name'],
                                        last_name=form.cleaned_data['last_name'])

    new_profile = Profile()
    new_profile.user = new_user
    new_profile.isChild = False
    new_profile.balance = 10000
    new_profile.savings = 10000
    new_profile.save()
    new_user.save()

    new_user = authenticate(username=form.cleaned_data['username'],
                            password=form.cleaned_data['password'])

    login(request, new_user)
    return redirect(reverse('global'))

@login_required
def addChild_action(request): 
    context = {}

    if request.method == 'GET':
        context['form'] = createChildForm()
        context['output'] = ''
        return render(request, 'createChild.html', context)

    # Creates a bound form from the request POST parameters and makes the 
    # form available in the request context dictionary.
    form = createChildForm(request.POST)
    context['form'] = form

    # Validates the form.
    if not form.is_valid():
        return render(request, 'createChild.html', context)

    # checks if the passwords are the same

    # if form.cleaned_data['password'] != form.cleaned_data['confirm_password']:
    #     return render(request, 'register.html', context)

    # At this point, the form data is valid.  Register and login the user.
    new_user = User.objects.create_user(username=form.cleaned_data['username'], 
                                        password=form.cleaned_data['password'],
                                        email=form.cleaned_data['email'],
                                        first_name=form.cleaned_data['first_name'],
                                        last_name=form.cleaned_data['last_name'])

    new_profile = Profile()
    new_profile.user = new_user
    new_profile.isChild = True
    new_profile.ParentProfile = request.user
    new_profile.rate = form.cleaned_data['saving_percentage']
    new_profile.save()
    new_user.save()

    
    return redirect(reverse('global'))

@login_required
def complete_chore(request, id):
    print("complete chore")
    parent = request.user.profile
    for chore in Chores.objects.all():
        if chore.id == id and not chore.complete: 
            child = chore.cid
            chore.complete = True
            child.savings = child.savings + Decimal(chore.reward * child.rate / 100)
            child.balance = child.balance + Decimal((chore.reward * (100 - child.rate)) / 100)
            parent.balance = parent.balance - Decimal(chore.reward)
            chore.save()
            child.save()
            parent.save()
            return global_action(request)
    return global_action(request)

@login_required
def complete_goal(request, id):
    child = request.user.profile
    parent = child.ParentProfile.profile
    for goal in Goals.objects.all():
        if goal.id == id and not goal.complete and child.balance >= goal.cost: 
            goal.complete = True
            child.balance = child.balance - Decimal(goal.cost)
            parent.balance = parent.balance + Decimal(goal.cost)
            goal.save()
            child.save()
            parent.save()
            return global_action(request)
    return global_action(request)

@login_required   
def addChore_action(request, id):
    context = {}
    if request.method == 'GET':
        context['form'] = addChoreForm()
        context['output'] = ''
        return render(request, 'addChore.html', context)

    form = addChoreForm(request.POST)
    context['form'] = form

    # Validates the form.
    if not form.is_valid():
        return render(request, 'addChore.html', context)

    chore = Chores()
    chore.chore = form.cleaned_data['chore']
    chore.reward = form.cleaned_data['award']
    chore.cid = get_object_or_404(User, id=id).profile
    chore.complete = False
    chore.save()
    return redirect(reverse('global')) # NEEDS LOOKED AT

@login_required
def addGoal_action(request, id):
    context = {}
    if request.method == 'GET':
        context['form'] = addGoalForm()
        context['output'] = ''
        return render(request, 'addGoal.html', context)

    form = addGoalForm(request.POST)
    context['form'] = form

    # Validates the form.
    if not form.is_valid():
        return render(request, 'addChore.html', context)

    goal = Goals()
    goal.goal = form.cleaned_data['goal']
    goal.cost = form.cleaned_data['cost']
    goal.cid = get_object_or_404(User, id=id).profile
    goal.complete = False
    goal.save()
    
    return redirect(reverse('global')) # NEEDS LOOKED AT

@login_required
def global_action(request):
    print(Chores.objects.all())
    if request.method == 'GET':
        context = {}
        if not request.user.profile.isChild:
            context['parent'] = request.user
            children = []
            for child in Profile.objects.all():
                print("hello")
                if child.isChild and child.ParentProfile == request.user:
                    childDict = {}
                    childDict.update({"child": child})
                    choresToDo = []
                    chorseComplete = []
                    for chore in Chores.objects.all():
                        print(chore.cid.user.id)
                        print(child.user.id)
                        if chore.cid == child:
                            print("found")
                            if not chore.complete:
                                choresToDo.append(chore)
                            else:
                                chorseComplete.append(chore)
                    childDict.update({'choresTodo': choresToDo})
                    childDict.update({'choresComplete': chorseComplete})
                    goalsTodo = []
                    goalsComplete = []
                    for goal in Goals.objects.all():
                        if goal.cid == child:
                            if goal.complete:
                                goalsComplete.append(goal)
                            else:
                                goalsTodo.append(goal)
                    childDict.update({'goalsComplete': goalsComplete})
                    childDict.update({'goalsTodo': goalsTodo})
                    children.append(childDict)
            context["childs"] = children
            return render(request, 'parent.html', context) 
        else:   
            context.update({"child": request.user})
            choresToDo = []
            chorseComplete = []
            for chore in Chores.objects.all():
                if chore.cid == request.user.profile:
                    if not chore.complete:
                        choresToDo.append(chore)
                    else:
                        chorseComplete.append(chore)
            context.update({'choresTodo': choresToDo})
            context.update({'choresComplete': chorseComplete})
            goalsTodo = []
            goalsComplete = []
            for goal in Goals.objects.all():
                if goal.cid == request.user.profile:
                    if goal.complete:
                        goalsComplete.append(goal)
                    else:
                        goalDict = {}
                        goalDict['goal'] = goal
                        balance = int(goal.cid.balance)
                        cost = goal.cost
                        progress = int(min(goal.cid.balance / goal.cost * 100, 100))
                        goalDict.update({'progress' : progress})
                        goalsTodo.append(goalDict)
            context.update({'goalsComplete': goalsComplete})
            context.update({'goalsTodo': goalsTodo})
            return render(request, 'child.html', context)

@login_required
def logout_action(request):
    logout(request)
    return redirect(reverse('login'))