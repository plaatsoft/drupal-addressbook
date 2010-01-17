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
This module requires the latest development version 4.X of Drupal.
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
16-09-2006 v4.x-0.1   
- Start programming 

20-09-2006 v4.x-0.2   
- Added addressbook view page

21-09-2006 v4.x-0.3   
- Added edit / delete family member
- Added drupal access roles

22-09-2006 v4.x-0.4   
- Added settings page.
- Improved birthday field
- Adding mobile number field
- Adding address (person) roles

23-09-2006 v4.x-0.5   
- Adding Dutch translation
- Owner of address can be changed by administrator
- Adding picture to address

24-09-2006 v4.x-0.6   
- Split view and edit page
- Adding country field
- Improve control button bar
- Add role filter in addressbook list page
- Added watchdog messages
- First Release

25-09-2006 v4.x-0.7   
- Picture upload dimensions check added
- Move pictures to /files/addressbook/ directory
- First name, Last name, Address and Street start always with upper character.
- Improve email field validation

26-09-2006 v4.x-0.8   
- Layout improved. Picture location improved!

27-09-2006 v4.x-1.0   
- Add family and member page (Redesign from 1 to 3 level hierarchy)

29-09-2006 v4.x-1.1   
- Input validation and navigation improved.

30-09-2006 v4.x-1.2   
- Add picture upload information.
- Default email address is filled in when creating a new family member.
- URL parameters are now checked before use in queries (security fix).

01-10-2006 v4.x-2.0   
- Improve navigation (Info pages are only showed when errors occur)
- Birthday is only showed when valid value is available.
- Improve error catching by SQL queries (security fix).

03-10-2006 v4.x-2.1   
- Add CSV upload.
- Add CSV download.	
- Remove minor bug in deleting member picture.
- Added build to the Drupal csv repository!  

04-10-2006 v4.x-2.2   
- Add birthday notification by email (cron hook)

05-10-2006 v4.x-2.3   
- Improve delete flow (Added "Are you Sure" dialog)
- Add wanted roles input
- Improve Filter (Selected filter value is store in session scope for later us)
- Bug fix: Now picture is always showed with the correct dimensions
- Add thumbnails pictures to family list and family member list.

07-10-2006 v4.x-2.4   
- Birthday email notification text is now configurable in setting page.
- Family list is now only showing one thumbnail per family. 
- Add search field in Family List page.
- Add more information to the Family member view page.

09-10-2006 v4.x-2.5   
- Add extra text by role input. So the functionality is clearer to the user!
- Add extra text for manitory input fields
- Manitory of email field can no be control in the setting page
- Bug fix: Family owner was something not set correctly. Bug solved!
- In family member list your can now search on members with Active or Interested in a role.

10-10-2006 v4.x-2.6   
- Add graphical map function.

16-10-2006 v4.x-2.7   
- Initial uploaded (CSV) Family and Family members do not have owner anymore!
- The first person who update a uploaded Family or Family member will be the owner.
- Bugfix: Now pictures are also showed when No clean URL's setting is used.
- Bugfix: Now active sort filter will stay active until user change it!
- Add today birthday block page. can be used to show everybody who is celebrating this birthday today.
- Add in to all addressbook page <div class="addressbook"> and </div> tag

28-10-2006 v4.x-2.8   
- Uploaded images are now automatic resize and thumbnails are created.
- Final version for Drupal 4
 
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

 