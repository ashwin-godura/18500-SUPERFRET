# Generated by Django 3.1.14 on 2022-08-12 15:28

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('PNC_Kids', '0007_auto_20220812_1522'),
    ]

    operations = [
        migrations.AlterField(
            model_name='profile',
            name='balance',
            field=models.DecimalField(decimal_places=2, max_digits=15),
        ),
        migrations.AlterField(
            model_name='profile',
            name='savings',
            field=models.DecimalField(decimal_places=2, max_digits=15),
        ),
    ]
