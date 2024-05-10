#!/bin/bash


# Howto translate the program to your own language:

# Run this command first to make the initial template.
  xgettext --no-location -k_:1 -kN_:1 -o gadmin-samba.pot ../src/*.c

# Copy the resulting gadmin-samba.pot as yourlang.po in the current directory.

# General information:
# Try to make the translated lines about as long as the english lines 
# (on some places it might otherwise expand the application too much).

# You dont have to translate all the country names from the credits dialog
# if you dont want to.

# Now translate yourlang.po in utf-8 with gedit or so and
# set it to the correct language, charcode and email in the .po file.
# (All the upper case names should be changed).

# If you have written the translation in gedit (utf8) .. then use
# CHARCODE=utf-8


# Now add your language to ALL_LINGUAS="sv en dk" in configure.in then
# run autoconf.

# Install it and check the result.

# If the result looks good then send me the .po file.
