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

function addressbook_sortbar($Sort,$Action)
{
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

/*
 * View name in formatted style
 * @ first_name
 * @ middle_name
 * @ last_name
 * @ short
 * @ return name formatted in HTML
 */
function view_name($first_name,$middle_name,$last_name,$short) {

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
function view_birthday($birthday_day,$birthday_month,$birthday_year,$readonly) {

   global $user;

   $page.='<tr><td valign="top" width=15%>';
   $page.=t('Birth Day');
   $page.='</td><td>';

   if ( $readonly == '0' ) {
     $page.='<select name="birthday_day">';
     for ($i=1; $i<32; $i++)
     {
        $page.='<option value="'.$i.'" ';
        if ($birthday_day==$i) $page.='selected="selected" ';
        $page.='>'.$i.'</option>';
     }
     $page.='</select> ';

     $month=array("",t('January'),t('February'),t('March'),t('April'),t('May'),
       t('June'),t('July'),t('August'),t('September'),t('October'),t('November'),t('December'));

     $page.='<select name="birthday_month">';
     for ($i=1; $i<13; $i++)
     {
        $page.='<option value="'.$i.'" ';
        if ($birthday_month==$i) $page.='selected="selected" ';
        $page.='>'.$month[$i].'</option>';
     }
     $page.='</select> ';

     if ( $readonly == '0' )
     {
       $current_year=date('Y');
       $page.='<select name="birthday_year">';
       for ($i=1900; $i<=$current_year; $i++) {
         $page.='<option value="'.$i.'" ';
         if ($birthday_year==$i) $page.='selected="selected" ';
         $page.='>'.$i.'</option>';
       }
       $page.='</select> ';
     }
   }
   else {
     $birtday .= $birthday_day.'-'.$birthday_month.'-'.$birthday_year;
     $page .= '<input id="text" name="birthday" size="10" type="text" value="'.$birtday.'" READONLY />';
   }

   $page.='</td></tr>';
   return $page;
}

/*
 * View Owner field (Only administrator can change owner field)
 * @ owner
 * @ return HTML
 */
function view_owner($uid, $readonly) {

   global $user;

   $page.='<tr><td valign="top" width=15%>';
   $page.=t('Owner');
   $page.='</td><td>';

   if ( $readonly=='0' ) {
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
     $page.='<option value="'.t('No owner').'" ';
     if ($uid==0) $page.='selected="selected" ';
     $page.='>'.t('No owner').'</option>';
     $page.='</select>';
   }
   else {
     if ($uid==0) {
       $page .= '<input id="text" name="owner" size="20" maxlength="20" type="text" value="'.t('No owner').'" READONLY />';
     } else
     {
       $query  = 'SELECT uid, name FROM {users} WHERE uid='.$uid;
       $queryResult = db_query($query);
       $tmp = db_fetch_object($queryResult);

       $page .= '<input id="text" name="owner" size="20" maxlength="20" type="text" value="'.$tmp->name.'" READONLY />';
     }
   }
   $page.='</td></tr>';
   return $page;
}

/**
 * Roles
 * This function will show all roles storing in the setting page.
 * @return HTML
 */
function view_roles($active_roles,$wanted_roles,$readonly) {

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
function view_country($country, $readonly) {

   $page='';

   $page.='<tr><td valign="top">';
   $page.=t('Country');
   $page.='</td><td>';

   if ($readonly=='0') {
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
   }
   else {
     $page .= '<input id="text" name="contry" size="50" maxlength="50" type="text" value="'.$country.'" READONLY />';
   }
   $page.='</td>';
   $page.='</tr>';
   return $page;
}

/*
 * Function fill table line
 * @return HTML
 */
function view_line($first,$second) {

   $page .= "<tr><td valign='top' width=15%>".$first."</td><td>".$second."</td></tr>\r\n";
   return $page;
}

/*
 * Function valid email address
 * @return true or false
 */
function check_mail($adres) {

   return ! ereg("[A-Za-z0-9_-]+([\.]{1}[A-Za-z0-9_-]+)*@[A-Za-z0-9-]+([\.]{1}[A-Za-z0-9-]+)+",$adres);
}

/*
 * Function valid number input
 * @returns true if valid number (only numeric string), false if not
 */
function check_number($str) {

  if (ereg('^[[:digit:]]+$', $str))
    return true;
  else
    return false;
}

/*
 * Function check user access
 * @returns true if access is allowed else false 
 */
function check_access($uid) {

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

/*
 * Function valid alfanumeric imput
 * @returns true if valid number (only alfanumeric string), false if not
 */
function hacker_warning() {

  //watchdog('user', 'Addressbook hacking attempt');
  $page .= '<br/>';
  $page .= '<h1>'.t('System Warning').'</h1>';
  $page .= t('Input is not valid! Please do not modify the URL manually').'<br/>';
  $page .= t('This hacking attemp is recorded!').'<br/>';
  $page .= '<br/>';
  $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  $page .= '<input type="submit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</div>';
  print theme("page", $page);
}