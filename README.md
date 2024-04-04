# TEAM 2 README

Welcome to Team 2's Paparazzi code.

Some changes in this fork include:

- Adding extra colour filters
- More efficient cv_detect_colour_object.c file
- Adding stronger weighting towards objects detected in middle columns of image
- Slicing image to central 60 rows

Our code aims to detect each object in the cyberzoo by specifying the colour range for each and using smooth flight to dodge each object.

The main files changed in this fork are:

cv_detect_color_object.c
orange_avoider.c
orange_avoider.xml
cv_detect_color_object.xml
bebop_course_orangeavoid.xml
