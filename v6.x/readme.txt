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
This module requires the latest development version 6.X of Drupal.
The GD library must be active in the PHP Apache module, or else images will not work!

Installation
------------
1. Copy the addressbook folder and its contents to the Drupal modules/ directory. 
   Drupal should automatically detect it and create the necessary database queries.
2. Go to 'administer -> modules' and enable addressbook.
3. Setting can be changed in 'Administer > Settings > Addressbook'
   Obtain a free www.map24.com AJAX API key, or else the map function will not work!
4. Populate database with an initial CSV upload file.
   CSV must have the following format (First line of the CSV input file must be this banner line):
   FIRST_NAME,MIDDLE_NAME,LAST_NAME,STREET,ZIPCODE,CITY,COUNTRY,TELEPHONE,MOBILE,EMAIL,BIRTH_DAY,WORK,NOTES,ACTIVE_ROLES,WANTED_ROLES
5. Create a new drupal menu which is pointing to the following URL http://!your URL!/addressbook.
   Now you can access the addressbook by this URL.
 
Release Notes
-------------
 
16-09-2009 v6.x-3.4
- Took addressbook v5.x-3.4 as baseline for this build
- Use file_directory_path() function instead of hardcoded /files definition.
- Improve address.info information.
- This is the first Addressbook release for Drupal 6.
- Updated drupal.org addressbook cvs repository for automatic update detection.
- Note: Sending birthday and group emails is not working yet. This will be solved in next release!

17-09-2009 v6.x-3.5
- Birthday and groups email functionality is working now fine!
- First stable release for Drupal 6

20-09-2009 v6.x-3.6
- Remove english typo's in module, documentation and source code.
- Thanks Stijn Bouwhuis for the correction!

23-10-2009 v6.x-3.7
- Hot security fix to protect against XSS (Cross Site Scripting) hacking.

29-05-2010 v6.x-4.0
- Added main menu.
- Splits functionality over more files.
- Added JaveScript and CSS file
- Added HTML based on active drupal template.
- Rebuild family page from cratch.
	- Insert Family. OK
	- Update Family. OK
	- Delete Family. ? (Clean up members and pictures)
- Rebuild member page from cratch.
	- Insert member. OK
 	- Update member. OK
	- Delete member. ? (Clean up pictures)
- Descope group email functionality.

05-06-2010 v6.x-4.1
- Improve member and family delete. Delete all related data.

Known Minor Issues
------------------
- Image is not removed when family member is deleted
- Image is not removed when family is deleted
- Role definitions may not contain spaces