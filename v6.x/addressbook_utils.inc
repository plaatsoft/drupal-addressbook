<?php

/** 
 *  Addressbook module for drupal 
 *
 *  Copyright (C) 2006-2010
 *  =======================
 *
 *  Created by wplaat
 *
 *  For more information visit the following website.
 *  Website : http://www.plaatsoft.nl 
 *
 *  Or send an email to the following address.
 *  Email   : info@plaatsoft.nl
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// ********************************************
// Debug methods
// ********************************************

/**
 * debug_param
 * Print on screen POSTED parameters
 */
function addressbook_debug_param($id, $param) {
	echo '['.$id.'='.$param.']';
}

/**
 * debug_param
 * Print on screen SQL commands
 */
function addressbook_debug_sql($sql) {
	echo '[Query='.$sql.']';
}


function addressbook_show_array($array) {
    foreach ($array as $value) {
        if (is_array($value)) {
            show_array($value);
        } else {
            echo $value . "<br>";
        }
    }
}

// ********************************************
// Debug methods
// ********************************************

function addressbook_view_timestamp($last_updated) {

	if ($last_updated!="") {
		list($year, $time) = split(' ', $last_updated);
		list($access_year, $access_month, $access_day) = split('[/.-]', $year);
		list($access_hour, $access_minute, $access_sec) = split(':', $time);
	
		$page = $access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;
	}
	return $page;
}

function addressbook_sortbar($Sort,$Action) {

	$sort_bar=array("A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P", "Q","R","S","T","U","V","W","X","Y","Z");
	for ($i=0; $i<26; $i++)
	{
		if ($i==0) $page .= '|';
		if ($Sort==$sort_bar[$i]) { 
			$page .= '<B>'.$Sort.'</B>|';
		} else {		
			$page .= oostpoort_hiddenlink('AddressbookForm', 'Sort', $sort_bar[$i], $Action, $sort_bar[$i]).'|';
		}
	}
	return $page;
}

/**
 * Create hidden link 
 */
function addressbook_hiddenlink($form,$key,$value,$action,$label) {
   return '<a href="#" onClick="hiddenInput(\''.$form.'\', \''.$key.'\', \''.$value.'\',\''.$action.'\');">'.$label.'</a>';
}

/** 
 * View text input field.
 */
function addressbook_view_text($id, $size, $maxSize, $value, $readonly) {
	
	if ($readonly) {
		return $value;
	} else {	
		return '<input id="text" name="'.$id.'" size="'.$size.'" maxlength="'.$maxSize.'" type="text" value="'.$value.'">';
	}
}

/** 
 * View textarea input field.
 */
function addressbook_view_textarea($id, $cols, $rows, $maxSize, $value, $readonly) {
	
	if ($readonly) {
		return $value;
	} else {	
		return '<textarea name="'.$id.'" cols='.$cols.' rows='.$rows.' maxlength='.$maxSize.'>'.$value.'</textarea>';
	}
}

/** 
 * Oostpoort form item.
 */
function addressbook_form_item($label,$value,$description, $readonly) {
	
	if ($value!="") {
		
		$page .= '<div class="form-item">';
		
		$page .= '<label>';
		$page .= $label;
		$page .= '</label>';
		
		$page .= $value;
		
		if (($description!="") && !$readonly) {
			$page .= '<div class="description">';
			$page .= $description;
			$page .= '</div>';
		}
		
		$page .= '</div>';
	}
	return $page;
}

/*
 * View name in formatted style
 * @ first_name
 * @ middle_name
 * @ last_name
 * @ short
 * @ return name formatted in HTML
 */
function addressbook_view_name($first_name,$middle_name,$last_name,$short) {

	if ($short) {
		$tmp=split(" ", $first_name);
		$i=0;
		$first_name='';
		while ($tmp[$i]!='') {
			$first_name.=ucfirst($tmp[$i++][0]).'.';
		}
	}

	if ( variable_get('addressbook_name_format',1)=='1') {
		$name = $first_name . ' ' . $middle_name . ' ' . $last_name;
	} else {
		$name = $middle_name . ' ' . $last_name. ', ' . $first_name;
	}
	return $name;
}


/*
 * View birthday field
 * @ birthday_day
 * @ birthday_month
 * @ birthday_year
 * @ return HTML
 */
function addressbook_view_birthday($birthday,$readonly) {

	list($birthday_year, $birthday_month, $birthday_day) = split('-', $birthday);
	
   if ( !$readonly ) {
	
		$page.='<select name="birthday_day">';
		for ($i=1; $i<32; $i++) {
        $page.='<option value="'.$i.'" ';
        if ($birthday_day==$i) $page.='selected="selected" ';
        $page.='>'.$i.'</option>';
		}
		$page.='</select> ';

		$month=array("",t('January'),t('February'),t('March'),t('April'),t('May'),
			t('June'),t('July'),t('August'),t('September'),t('October'),t('November'),t('December'));

		$page.='<select name="birthday_month">';
		for ($i=1; $i<13; $i++) {
			$page.='<option value="'.$i.'" ';
			if ($birthday_month==$i) $page.='selected="selected" ';
			$page.='>'.$month[$i].'</option>';
		}
		$page.='</select> ';

		$current_year=date('Y');
      $page.='<select name="birthday_year">';
      for ($i=1900; $i<=$current_year; $i++) {
         $page.='<option value="'.$i.'" ';
         if ($birthday_year==$i) $page.='selected="selected" ';
         $page.='>'.$i.'</option>';
      }
      $page.='</select> ';
   
	} else {
     
		$page .= $birthday_day.'-'.$birthday_month.'-'.$birthday_year;
   }
   return $page;
}

/*
 * View Owner field (Only administrator can change owner field)
 * @ owner
 * @ return HTML
 */
function addressbook_view_owner($uid, $readonly) {

   global $user;

   if ( ! $readonly ) {
	
     $query  = 'SELECT uid, name FROM {users}';
     $queryResult = db_query($query);

     // Workaround Drupal first entry in users table is always empty. Skip this entry
     $tmp = db_fetch_object($queryResult);

     $page.='<select name="owner">';
     while ($tmp = db_fetch_object($queryResult)) {
        $page.='<option value="'.$tmp->uid.'" ';
        if ($tmp->uid==$uid) $page.='selected="selected" ';
        $page.='>'.$tmp->name.'</option>';
     }
     $page.='<option value="0" ';
     if ($uid==0) $page.='selected="selected" ';
     $page.='>'.t('No owner').'</option>';
     $page.='</select>';
	  
   } else {
	
		if ($uid==0) {
	  
			$page .= t('No owner');
		
		} else {
	  
			$query  = 'SELECT uid, name FROM {users} WHERE uid='.$uid;
			$queryResult = db_query($query);
			$tmp = db_fetch_object($queryResult);

			$page .= $tmp->name;
		}
   }
   return $page;
}

/**
 * Roles
 * This function will show all roles storing in the setting page.
 * @return HTML
 */
function addressbook_view_roles($active_roles,$wanted_roles,$readonly) {

   global $user;
   $page='';

   $roles=split( ",", variable_get('addressbook_roles',''));
   if ($roles[0]=='')
   {
      // No roles defined in setting page. return directly!
      return $page;
   }

   if (($readonly=='1') && ($active_roles=='') && ($wanted_roles=='')) {
      // Nothing to show
      return $page;
   }

   if ( $readonly=='0' ) {
     $page .= '<br/>';
     $page .= '<b>'.t('Please enter the roles you currently fulfilling and please enter the roles you are interesed in now or in the near future!').'</b><br/>';
     $page .= '<br/>';
   }

   $page .= '<table border=0 width=100% cellpadding=1>';
   $page .='<tr>';
   $page .='<td valign="top" width=15%>';
   $page .=t('Active Roles').'&nbsp;';
   $page .='</td><td width=35%>';

   $first=1;
   $i=0;
   if ( $readonly=='0' ) {
     while ($roles[$i]!='') {

       if ( !strstr( $active_roles, $roles[$i] ) ) {
          $page.='<input type="checkbox" name="active_'.trim($roles[$i]).'">'.$roles[$i].'<br/>';
       }
       else {
         $page.='<input type="checkbox" name="active_'.trim($roles[$i]).'" CHECKED>'.$roles[$i].'<br/>';
       }
       $i++;
     }
   }
   else {
     while ($roles[$i]!='') {
       if ( strstr( $active_roles, $roles[$i] ) ) {
         $page.='<input type="checkbox" name="active_'.trim($roles[$i]).'" CHECKED DISABLED>'.$roles[$i];
         $page.='</input>';
         $page.='<br/>';
       }
       $i++;
     }
   }

   $page.='</td>';
   $page.='<td valign="top" width=15%>';
   $page.=t('Interesing Roles').'&nbsp;';
   $page.='</td>';
   $page.='<td width=35%>';

   $first=1;
   $i=0;
   if ( $readonly=='0' ) {
     while ($roles[$i]!='') {

       if ( !strstr( $wanted_roles, $roles[$i] ) ) {
          $page.='<input type="checkbox" name="wanted_'.trim($roles[$i]).'">'.$roles[$i].'<br/>';
       }
       else {
         $page.='<input type="checkbox" name="wanted_'.trim($roles[$i]).'" CHECKED>'.$roles[$i].'<br/>';
       }
       $i++;
     }
   }
   else {
     while ($roles[$i]!='') {
       if ( strstr( $wanted_roles, $roles[$i] ) ) {
         $page.='<input type="checkbox" name="wanted_'.trim($roles[$i]).'" CHECKED DISABLED>'.$roles[$i];
         $page.='</input>';
         $page.='<br/>';
       }
       $i++;
     }
   }

   $page.='</td>';
   $page.='</tr>';
   $page.='</table>';
   return $page;
}


/**
 * View Country
 * This function will show all available countries.
 * @return HTML
 */
function addressbook_view_country($country, $readonly) {

   if (! $readonly) {
     $countries=array( 'Afghanistan','Albania', 'Algeria', 'American Samoa', 'Andorra', 'Angola', 'Anguilla',
      'Antigua and Barbuda', 'Argentina', 'Armenia',  'Ascension Island', 'Australia','Austria',
      'Azerbaijan', 'Bahamas', 'Bahrain',  'Bangladesh',   'Barbados',  'Belarus',  'Belgium',
      'Belize',  'Benin',    'Bermuda',    'Bhutan',    'Bolivia',    'Bosnia and Herzegovina',
      'Botswana',    'Brazil',    'British Indian Ocean Territory',    'Brunei Darussalam',
      'Bulgaria',    'Burkina Faso',    'Burundi',    'Camaroon',    'Cambodia',    'Cameroon',
      'Canada',    'Cape Verde',    'Cayman Islands',    'Central African Republic',    'Chad',
      'Chile',    'China',    'Colombia',    'Comoros',    'Congo',    'Cook Islands',    'Costa Rica',
      'Cote D Ivoire',    'Croatia',    'Cuba',    'Cyprus',    'Czech Republic',    'Denmark',    'Djibouti',
      'Dominica',    'Dominican Republic',    'Ecuador',    'Egypt',    'El Salvador',    'Equatorial Guinea',
      'Eritrea',    'Estonia',    'Ethiopia',    'Falkland Islands',    'Faroe Islands',    'Federated States of Micronesia',
      'Fiji',    'Finland',    'France',    'French Guiana',    'French Polynesia',    'Gabon',   'Georgia',
      'Germany',    'Ghana',    'Greece',    'Greenland',    'Grenada',    'Guadeloupe',    'Guatemala',
      'Guam',    'Guinea',    'Guinea Bissau',    'Guyana',    'Haiti',    'Honduras',    'Hungary',
      'Iceland',    'India',    'Indonesia',    'Iran',    'Ireland',    'Isle of Man',    'Israel',
      'Italy',    'Jamaica',    'Japan',    'Jordan',    'Kazakhstan',    'Kenya',    'Kiribati',    'Korea',
      'Kuwait',    'Kyrgyzstan',    'Laos',    'Latvia',    'Lebanon',    'Lesotho',    'Liberia',
      'Libya',    'Liechtenstein',    'Lithuania',    'Luxembourg',    'Macau',    'Macedonia',
      'Madagascar',    'Malawi',    'Malaysia',    'Maldives',    'Mali',    'Malta',    'Marshall Islands',
      'Martinique',    'Mauritius',    'Mayotte',    'Mexico',    'Moldova',    'Monaco',    'Mongolia',
      'Montenegro',    'Montserrat',    'Morocco',    'Mozambique',    'Myanmar',    'Namibia',
      'Nauru',    'Nepal',    'Nederland',    'Netherlands Antilles',    'New Caledonia',    'New Zealand',
      'Nicaragua',    'Niger',    'Nigeria',    'Niue',    'Norfolk Island',    'Northern Mariana Islands',
      'Norway',    'Oman',    'Pakistan',    'Palau',    'Panama',    'Papua New Guinea',    'Paraguay',
      'Peru',    'Philippines',    'Pitcairn',    'Poland',    'Portugal',    'Puerto Rico',    'Qatar',
      'Reunion',    'Romania',    'Russian Federation',    'Rwanda',    'Saint Vincent and the Grenadines',
      'San Marino',    'Sao Tome and Principe',    'Saudi Arabia',    'Senegal',    'Serbia',    'Seychelles',
      'Sierra Leone',    'Singapore',    'Slovakia',    'Slovenia',    'Solomon Islands',    'Somalia',
      'South Africa',    'South Georgia',    'Spain',    'Sri Lanka',    'St. Kitts and Nevis',    'St. Lucia',
      'St. Pierre and Miquelon',    'Sudan',    'Suriname',    'Swaziland',    'Sweden',
      'Switzerland',    'Syrian Arab Republic',    'Taiwan',    'Tajikistan',    'Tanzania',
      'Thailand',    'The Gambia',    'Togo',    'Tokelau',    'Tonga',    'Trinidad and Tobago',
      'Tunisia',    'Turkey',    'Turkmenistan',    'Turks and Caicos Islands',    'Tuvalu',
      'Uganda',    'Ukraine',    'United Arab Emirates',    'United Kingdom',    'United States',
      'Uruguay',    'Uzbekistan',    'Vanuatu',    'Venezuela',    'Viet Nam',    'Virgin Islands',
      'Western Samoa',    'Yemen',    'Yugoslavia',    'Zaire',    'Zambia',    'Zimbabwe');

      $page .='<select name="country">';
      for ($i=1; $i<213; $i++) {
         $page.='<option value="'.$countries[$i].'" ';
         if ($country==$countries[$i]) $page.='selected="selected" ';
         $page.='>'.$countries[$i].'</option>';
      }
      $page.='</select> ';
		
   } else {
	
     $page .= $country;
	  
   }
   return $page;
}

// ********************************************
// Other
// ********************************************

/**
 * View Date Reverse type
 */
function addressbook_convert_date_reverse($date) { 

  $page = "";
  
  if ($date!="") {
		list($day, $month, $year) = split('[/.-]', $date);
		$page = $year.'-'.$month.'-'.$day;
  }
  return $page;
}


/*
 * Function valid email address
 * @return true or false
 */
function addressbook_check_mail($adres) {

   return ! ereg("[A-Za-z0-9_-]+([\.]{1}[A-Za-z0-9_-]+)*@[A-Za-z0-9-]+([\.]{1}[A-Za-z0-9-]+)+",$adres);
}

/*
 * Function valid number input
 * @returns true if valid number (only numeric string), false if not
 */
function addressbook_check_number($str) {

  if (ereg('^[[:digit:]]+$', $str))
    return true;
  else
    return false;
}

/*
 * Function check user access
 * @returns true if access is allowed else false 
 */
function addressbook_check_access($uid) {

  global $user;
  if ( ( (  ($user->uid==$uid) || 
            ($uid==0) ||  
            (variable_get('addressbook_wiki_edit_mode',0)==1)
         ) 
         && user_access('add address')
        ) 
        || user_access('access administration pages') 
     ) 
  {
    return true;
  }
  else
  {
    return false;
  }
}


/**
 * Render a page showing the picture full screen.
 * @return HTML
 */
function addressbook_picture_view( $pid=0 ) {

  global $user;
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook view picture') );

  // Validate HTTP URL parameter
  if ( !check_number($pid) )
  {
     return hacker_warning();
  }

  // get fid from session scope
  $fid=$_SESSION["fid"];
  // get fid from session scope
  $mid=$_SESSION["mid"];

  $picture_delete = isset($_POST['picture_delete']) ? '1' : '0';
  $go_delete = isset($_POST['go_delete']) ? '1' : '0';

  // Check access rights
  $query  = 'SELECT uid, picture FROM {addressbook_picture} WHERE pid='.$pid;
  $queryResult = db_query($query);
  $tmp = db_fetch_object($queryResult);

  // If no data has been found. Show anti hacking message
  if ($tmp==null) {
    return hacker_warning();
  }

  // Delete 'Are you sure?' form
  if (($picture_delete=='1') && ($pid!='0') && ($go_delete=='0')) {
    $page .= '<br/>';
    $page .= t('Are you sure you want to delete the picture?');
    $page .= '<br/>';
    $page .= '<br/>';
    $page .= '<table align="left">';
    $page .= '<tr><td>';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="go_delete" value="'.t('Yes').'" />';
    $page .= '<input type="hidden" name="picture_delete" />';
    $page .= '</form>';
    $page .= '</td><td>';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="cancel_delete" value="'.t('No').'" />';
    $page .= '</form>';
    $page .= '</td></tr>';
    $page .= '</table>';
    $page .= '</div>';
    return $page;
  }

  // Delete selected picture from disk and database
  if (($picture_delete=='1') && ($pid!='0') && ($go_delete=='1')) {

     if (is_file(IMAGE_DIR.'/'.$tmp->picture)) {
       unlink(IMAGE_DIR.'/'.$tmp->picture);
     }
     if (is_file(THUMBNAILS_DIR.'/'.$tmp->picture)) {
       unlink(THUMBNAILS_DIR.'/'.$tmp->picture);
     }

     $query = 'DELETE FROM {addressbook_picture} WHERE pid='.$pid;
     $result = db_query($query);
     if ($result==1)
     {
       // Query succesfull
       watchdog('user', 'Picture '.$pid.' is deleted from the addressbook');

       $page .= '<td valign="top">';
       $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
       $page .= '<input type="submit" name="picture_delete" value="'.t('Delete').'" />';
       $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
       $page .= '</form>';
       $page .= '</td>';

       if ($mid!='0') {
         if (check_access($tmp->uid)) {
           return member_edit($mid);
         }
         else {
           return member_view($mid);
         }
       }
       else {
          return family_view2($fid);
       }
     }
     else {
       // Query failed
       $page .= '<br/>';
       $page .= t('Failed to delete picture, please try again');
       $page .= '<br/>';
       $page .= '<br/>';
       if ($mid!='0') {
         if (check_access($tmp->uid)) {
           $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
         }
         else {
           $page .= '<form action="'.url(URL_MEMBER_VIEW.'/'.$mid).'" method="POST">';
         }
       }
       else {
          $page .= '<form action="'.url(URL_FAMILY_VIEW.'/'.$fid).'" method="POST">';
       }

       $page .= '<input type="submit" value="'.t('Return').'" />';
       $page .= '</form>';

       $page .= '</div>';
       return $page;
     }
  }

  // Show menu Bar
  $page .= '<br/>';
  $page .= '<table align="left">';
  $page .= '<tr>';

  if ( check_access($tmp->uid) ) {
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_PICTURE_VIEW.'/'.$pid).'" method="POST">';
    $page .= '<input type="submit" name="picture_delete" value="'.t('Delete').'" />';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '</form>';
    $page .= '</td>';
  }

  $page .= '<td valign="top">';

  if ($mid==0)
  {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/'.$fid).'" method="POST">';
  }
  else
  {
     if (check_access($tmp->uid)) {
        $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
     }
     else {
        $page .= '<form action="'.url(URL_MEMBER_VIEW.'/'.$mid).'" method="POST">';
     }
  }
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '<input type="submit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</td>';
  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<br/>';

  if ($picture_delete=='0') {
    $filename=url(IMAGE_DIR.'/'.$tmp->picture);
    $filename=str_replace(array("?q="), "", $filename);
    $page.='<img src="'.$filename.'"/>';
  }
  $page .= '<br/>';

  $page .= '</div>';
  print theme("page", $page);
}

/**
 * Render a page for uploading a picture.
 * @return HTML
 */
function addressbook_picture_edit( $mid=0 ) {

  global $user;
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook edit picture') );

  // Validate HTTP URL parameter
  if ( !check_number($mid) )
  {
     return hacker_warning();
  }

  //
  // Fetch data from HTTP request
  //
  $fid=$_POST['fid'];
  if ( !check_number($fid) )
  {
     return hacker_warning();
  }

  // Check access rights
  $query  = 'SELECT uid FROM {addressbook_family} WHERE fid='.$fid;
  $queryResult = db_query($query);
  $tmp = db_fetch_object($queryResult);

  // f no data has been found. Show anti hacking message
  if ($tmp==null) {
    return hacker_warning();
  }

  if ( !check_access($tmp->uid) ) {
    // Deny access, no more clues.
    // Everything will be in the watchdog's URL for the administrator to check.
    drupal_access_denied();
    return;
  }

  // Fetch Image upload in a HTTP request
  if ($_FILES['uploadedfile']['name']!='') {
    if ( (strstr($_FILES['uploadedfile']['name'],'.jpg')) || (strstr( $_FILES['uploadedfile']['name'],'.JPG')) )
    {
      $error='0';
      if(!is_dir(IMAGE_DIR))
      {
        if( !mkdir(IMAGE_DIR) )
        {
          $page .= '<br/>';
          $page .= t('Creating image directory failed!');
          $page .= '<br/>';
          $error='1';
        }
      }
      $filename= date('YmdHis', time()).'.jpg';

       // Check dimensions of image
      $dimensions = getimagesize($_FILES['uploadedfile']['tmp_name']);
      $width  = $dimensions[0];
      $height = $dimensions[1];
      $resize = 0;

      // Limit picture 
      if ($width>$height)
      {
         $ratio = 800 / $width;
         if ($ratio < 1)
         {
            $width=800;
            $height=$height*$ratio;
            $resize = 1;
         }
      }
      else {
         $ratio = 600 / $width;
         if ($ratio < 1)
         {
            $width=600;
            $height=$height*$ratio;
            $resize = 1;
         }
      }

      $page .= '<br/>';

      // Resize uploaded picture
      if ( $resize == 1 )
      {
         if (!image_scale($_FILES['uploadedfile']['tmp_name'], file_create_path(IMAGE_DIR.'/'.$filename), $width, $height)) {
              $page .= '<br>';
              $page .= t('Unable to create image. Is Gd library active in apache php module?');
              $error='1';
              $page .= '<br>';
         }
         else
         {
            chmod(IMAGE_DIR.'/'.$filename, 0666);
         }

      }
      else
      {
        // No resize needed, move file to storage 
        copy($_FILES['uploadedfile']['tmp_name'],IMAGE_DIR.'/'.$filename);
      }

      // Create thumbnails directory store
      if(!is_dir(THUMBNAILS_DIR))
      {
        if( !mkdir(THUMBNAILS_DIR) )
        {
          $page .= '<br/>';
          $page .= t('Creating image directory failed!');
          $page .= '<br/>';
          $error='1';
        }
      }

      $page .= '<br/>';

      $resize = 0;

      // Limit picture size on screen
      if ($width>$height)
      {
         $ratio  = 160 / $width;
         if ($ratio < 1)
         {
            $width=160;
            $height=$height*$ratio;
            $resize = 1;
         }
      }
      else
      {
         $ratio  = 120 / $width;
         if ($ratio < 1)
         {
            $width=120;
            $height=$height*$ratio;
            $resize = 1;
         }
      }

      // Create thumbnail picture
      // Resize uploaded picture
      if ( $resize == 1 )
      {
        if (!image_scale(IMAGE_DIR.'/'.$filename, file_create_path(THUMBNAILS_DIR.'/'.$filename), $width, $height)) {
          $page .= '<br>';
          $page .= t('Unable to create thumbnails image. Is the GD library active in the Apache PHP module?');
          $error='1';
          $page .= '<br>';
        }
        else
        {
            chmod(THUMBNAILS_DIR.'/'.$filename, 0666);
        }
      }
      else
      {
        // No resize needed, move file to storage 
        copy(IMAGE_DIR.'/'.$filename, THUMBNAILS_DIR.'/'.$filename);
      }

      if ($error=='0') {
        // Save picture in database
        $query  = 'INSERT INTO addressbook_picture (fid,mid,picture,uid,last_updated) VALUES (';
        $query .= '"'.$fid.'",';
        $query .= '"'.$mid.'",';
        $query .= '"'.$filename.'",';
        $query .= '"'.$user->uid.'",';
        $query .= 'SYSDATE() )';
        $result = db_query($query);
        if ($result==1)
        {
           // Query Succesfull
           watchdog('user', 'A picture has been added to address '.$id.' in addressbook');

           if ($mid=='0')
           {
              // Return to family view page
              return family_view2($fid);
           }
           else {
              return member_edit($mid);
           }
        }
        else {
          // Query Failed
          $page .= '<br/>';
          $page .= t('Failed to add picture, please try again!');
          $page .= '<br/>';
        }
      }
    }
    else {
      $page .= '<br/>';
      $page.=t('Only the jpg image format is supported for pictures');
      $page .= '<br/>';
      $_FILES['uploadedfile']['name']='';
    }
  }

  if ($_FILES['uploadedfile']['name']=='') {
    $page .= '<form enctype="multipart/form-data" action="'.url(URL_PICTURE_EDIT.'/').$mid.'" method="POST">';
    $page .= '<br/>';
    $page .= '<input type="hidden" />';
    $page .= t('Choose a file to upload:').' <input name="uploadedfile" type="file" />';

    $page .= '<br/>';
    $page .= '<div id="mission">';
    $page .= t('Warning: Only the jpg picture format is supported.');
    $page .= '<br/>';
    $page .= t('Image size must not exceed ').ini_get('upload_max_filesize').'B';
    $page .= '<br/></div>';
    $page .= '<br/>';

    // Show menu Bar
    $page .= '<table>';
    $page .= '<tr><td>';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="submit" value="'.t('Upload File').'" />';
    $page .= '</form>';
    $page .= '</td><td>';

    if ($mid=='0')
    {
      // Return to family view page
      $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
      $page .= '<input type="submit" value="'.t('Cancel').'" />';
      $page .= '</form>';
    }
    else {
      // Return to member view page
      $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
      $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
      $page .= '<input type="submit" value="'.t('Cancel').'" />';
      $page .= '</form>';
    }
    $page .= '</td></tr>';
    $page .= '</table>';
  }
  else
  {
    $page .= '<br/>';
    if ($mid=='0')
    {
      // Return to family view page
      $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
      $page .= '<input type="submit" value="'.t('Return').'" />';
      $page .= '</form>';
    }
    else {
      // Return to member view page
      $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
      $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
      $page .= '<input type="submit" value="'.t('Return').'" />';
      $page .= '</form>';
    }
  }

  $page .= '</div>';
  print theme("page", $page);
}

// ********************************************
// The end
// ********************************************
