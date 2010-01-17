// $Id: readme.txt,v 1.26 2009/09/16 18:41:09 wplaat Exp $

Created by wplaat (Plaatsoft)

This software is open source and may be copied, distributed or modified under the terms of the GNU General Public License (GPL) Version 2
 
For more information visit the following website.
Website : http://www.plaatsoft.nl 
 
Or send an email to the following address.
Email   : info@plaatsoft.nl

This module contains a simple addressbook.

Key features
------------
 - Standard storage of family and family member information
 - JPG picture can be added to contact information (requires GD library) 
 - Access to information is protected by standard drupal access roles
 - Family member roles can be added (added search on) 
 - CSV file upload / download of contact information
 - Birthday notification by email (cron job runs every day round 00:00:30 )
 - Graphical map integration through www.map24.com
 - Search within family member database only
 
Requirements
------------
This module requires the latest development version 5.X of Drupal.
And the GD library must be active in the PHP Apache module, or else images will not work!

Installation
------------

1. Copy the addressbook folder and its contents to the Drupal modules/ directory. 
   Drupal should automatically detect it and create the necessary database queries.

2. Go to 'administer -> modules' and enable activeselect.

3. Setting can be changed in 'Administer > Settings > Addressbook'
   Obtain a free www.map24.com AJAX API key, or else the map function will not work!

4. Populate database with an initial CSV upload file.
   CSV must have the following format (First line of the CSV input file must be this banner line):
 
   FIRST_NAME,MIDDLE_NAME,LAST_NAME,STREET,ZIPCODE,CITY,COUNTRY,TELEPHONE,MOBILE,EMAIL,BIRTH_DAY,WORK,NOTES,ACTIVE_ROLES,WANTED_ROLES

5. Create a new drupal menu which is pointing to the following URL http://!your URL!/addressbook/family/list.
   Now you can access the addressbook by this URL.
 
Release Notes
-------------

 Created by Willem van der Plaat (The Netherlands)
 
History:
 
31-01-2007 v5.x-3.0   
- Took addressbook v4.x-2.8 as baseline for this build
- Make module compliant with Drupal 5.1

12-06-2007 v5.x-3.1   
- Bugfix: Small pictures were not correct convert to thumbnail picture.

30-03-2008 v5.x-3.2   
- Add visible Member ID for administrator users.
- Improve birthday email notification

18-01-2009 v5.x-3.3   
- Bugfix: Do not send birthday email if person is older then 100 years.

15-09-2009 v5.x-3.4   
- Use file_directory_path() function instead of hardcoded /files definition.
- Improve address.info information
- Final version for Drupal 5

23-10-2009 v5.x-3.5
- Hot security fix to protect against XSS (Cross Site Scripting) hacking.

 
Known Minor Issues
------------------
- Image is not removed when family member is deleted
- Image is not removed when family is deleted
- Family members are not removed when family is deleted
- Role definitions may not contain spaces
 
Nice to have
------------
- Connect Birthday notification with event module.
- Special node which contains all members that celebrate their birthday in the current week.

 