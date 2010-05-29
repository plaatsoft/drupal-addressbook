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

// *********************************************************************************
// StateMachine
// *********************************************************************************
 
function addressbook_member($PrsId=0) {

	echo var_dump($_POST);
	
	// *****************************
	// Process POST an SESSION variables
	// *****************************
	
	$Action=htmlentities($_POST['Action']);
	
	// *****************************
	// Process statemachine
	// *****************************
	
	switch ($Action) {
   
		case 'MemberList':
			$page = addressbook_member_list();
			break;
			
		case 'FamilyView':
			$page = addressbook_family_view();
			break;
			
		case 'FamilyEdit':
			$page = addressbook_family_edit();
			break;
			
		case 'FamilyInsert':
			addressbook_family_insert();
			$page = addressbook_family_view();
			break;
			
		case 'FamilyUpdate':
			addressbook_family_update();
			$page = addressbook_family_view();
			break;
			
		case 'FamilyDelete':
			addressbook_family_delete();
			$page = addressbook_family_list();
			break;
			
		default: 
			$page = addressbook_member_list();
			break;
	}
	print theme("page", $page);
}

// ************************************
// Views
// ************************************

/**
 * Render a page showing the selected address for editing
 * @return HTML
 */
function addressbook_member_list( $sort='' )  {

	// *****************************
	// Get posted variables
	// *****************************
	
   $Sort = $_POST["Sort"];
	if ($Sort=="") {
		$Sort=$_SESSION["Sort"];
		if ($Sort=="") {
			$Sort="A";
		}
	}
	$_SESSION["Sort"]=$Sort;

	$Search=$_POST["Search"];

	$role = isset($_POST['roles']) ? $_POST['roles'] : '0';
	if ($role=='0') {
		$role=$_SESSION["role"];
	} else {
		$_SESSION["role"]=$role;
	}

  if (isset($_POST['role_type'])) {
		$_SESSION["role_type"]=$_POST['role_type'];
  }
  
	// *****************************
	// Query Database
	// *****************************
	
	if ($Search!="") {
		$parameter=$Search;
	} else {
	   $parameter=$Sort;
	}
	
	// Fetch member from database
  $query  = '
		SELECT 
			first_name, 
			middle_name, 
			last_name, 
			mobile, 
			email, 
			mid, 
			fid, 
			uid, 
			(SELECT telephone FROM {addressbook_family} WHERE fid=addressbook_member.fid) telephone 
		FROM 
			{addressbook_member}
		WHERE 
			last_name LIKE "'.$parameter.'%" ';
  
	if ($role!='') {
		if ($_SESSION['role_type']=='active') {
			$query .= 'AND active_roles LIKE "%'.$role.'%" ';
		} else {
			$query .= 'AND wanted_roles LIKE "%'.$role.'%" ';
		}
	}
	$query .= 'ORDER BY last_name, middle_name ';
	$queryResult = db_query($query);

	// *****************************
	// View
	// *****************************
		
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook member list') );  

  // Filter on Role
  $i=0;
  $page .= '<br/>';
  $page .= '<table border=0 width=100%>';
  $page .= '<tr>';

  $roles=split( ",", variable_get('addressbook_roles',''));
  if ($roles[0]!='') {
    $page .= '<td>';
    $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
    $page .= '<b>'.t('Filter by role: ').'</b>';

    $page .= '<select name="role_type" >';
    $page .= '<option value=active ';
    if ($_SESSION['role_type']=='active') $page.='selected="selected" ';
    $page.='>'.t('Active').'</option>';
    $page .= '<option value=wanted ';
    if ($_SESSION['role_type']=='wanted') $page.='selected="selected" ';
    $page.='>'.t('Interesed in').'</option>';
    $page .='</select> ';

    $page .= '<select name="roles" value="'.$role.'">';
    $page .='<option value=""></option>';
    while ($roles[$i]!='')
    {
      $page.='<option value="'.$roles[$i].'" ';
      if ($role==$roles[$i]) $page.='selected="selected" ';
      $page.='>'.$roles[$i].'</option>';
      $i++;
    }
    $page .='</select> ';
    $page .= '<input type="submit" value="'.t('Filter').'" />';
    $page .='</form>';
    $page .= '</td>';
  }

  $page .= '<td ALIGN="right">';
  $page .= '<form action="'.url(URL_EMAIL.'/'.$sort).'" method="POST">';
  $page .= '<input type="submit" value="'.t('Send group email').'" />';
  $page .= '</form>';

  $page .= '<td ALIGN="right">';
  $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  $page .= '<input type="submit" value="'.t('Go to family list').'" />';
  $page .= '</form>';
  $page .= '</td></tr>';
  //$page .= '</table>';

  //$page .= '<br/>';

  // Show sort bar (if enable in settings)
  //$page .= '<table border=0 >';
  $page .= '<tr>';
  if ( variable_get('addressbook_show_sort_bar',1)=='1') {
    $page .= '<td ALIGN="left">';
    $sort_bar=array("A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P",
     "Q","R","S","T","U","V","W","X","Y","Z");
    if ($sort=='') $page .= '<B>';
    $page .= '<a href="'.url(URL_MEMBER_LIST).'">'.t('ALL').'</a>|';
    if ($sort=='') $page .= '</B>';
    for ($i=0; $i<26; $i++)
    {
       if ($sort==$sort_bar[$i]) $page .= '<B>';
       $page .= '<a href="'.url(URL_MEMBER_LIST.'/'.$sort_bar[$i]).'">'.$sort_bar[$i].'</a>|';
       if ($sort==$sort_bar[$i]) $page .= '</B>';
    }
    $page .= '</td>';
  }
  $page .= '</tr>';
  $page .= '</table>';


  
  //$page .= '<br/>';
  $page .= '<table width=100% border=1 cellpadding=3 cellspacing=3>';

  // Show Banner
   // Only administrators can access the csv upload functionality
  $page .= '<tr><td>';
  if (user_access('access administration pages')) 
  {
     $page .= '<b>'.t('Id').'</b>';
     $page .= '</td><td>';
  }

  if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
     $page .= '<b>'.t('Picture').'</b>';
     $page .= '</td><td>';
  }
  $page .= '<b>'.t('Name').'</b>';
  $page .= '</td><td>';
  $page .= '<b>'.t('Telephone').'</b>';
  $page .= '</td><td>';
  $page .= '<b>'.t('Mobile').'</b>';
  $page .= '</td><td>';
  $page .= '<b>'.t('Email').'</b>';
  $page .= '</td></tr>';

  // Show all found members
  $page_tmp='';
  while ($data = db_fetch_object($queryResult))
  {
     $page_tmp .= '<tr><td width=50>';
     if (user_access('access administration pages')) 
     {
        $page_tmp .= $data->mid;
        $page_tmp .= '</td><td>';
     }

     if ( variable_get('addressbook_show_thumbnails',0)==1) {
      $query2  = 'SELECT pid, picture ';
      $query2 .= 'FROM {addressbook_picture} ';
      $query2 .= 'WHERE fid="'.$data->fid.'" AND mid="'.$data->mid.'"';
      $queryResult2 = db_query($query2);
      $tmp = db_fetch_object($queryResult2);
      if ($tmp!=null) {
        // Show each picture with is found
        if ( check_access($data->uid) ) {
           $page_tmp .= '<a href="'.url(URL_MEMBER_EDIT.'/').$data->mid.'">';
        }
        else {
           $page_tmp .= '<a href="'.url(URL_MEMBER_VIEW.'/').$data->mid.'">';
        }
        $filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
        $filename=str_replace(array("?q="), "", $filename);
        $page_tmp .='<img src="'.$filename.'" width=50 height=60/></a>';
        $page_tmp .= '</td><td>';
      }
      else {
        $page_tmp .= '</td><td>';
      }
     }

    // Format Name
    $name=view_name($data->first_name,$data->middle_name,$data->last_name,true);
    if ( check_access($data->uid) ) {
      $page_tmp .= l($name, URL_MEMBER_EDIT.'/'.$data->mid);
    }
    else {
      $page_tmp .= l($name, URL_MEMBER_VIEW.'/'.$data->mid);
    }
    $page_tmp .= '</td><td>';
    $page_tmp .= $data->telephone;
    $page_tmp .= '</td><td>';
    $page_tmp .= $data->mobile;
    $page_tmp .= '</td><td>';
    $page_tmp .= l($data->email,'mailto:'.$data->email);
    $page_tmp .= '</td></tr>';
  }

  if ( $page_tmp!='') {
     // Show Content
     $page .= $page_tmp;
  }
  else {
    // No content found
    $page .= '<tr><td>';
    $page .= t('No records found');
    $page .= '</td><td>';
    $page .= '</td><td>';
    $page .= '</td><td>';
    $page .= '</td></tr>';
  }
  $page .= '</table>';

  $page .= '</div>';
  
  print $page;
}

/**
 * Render a page showing the selected member in readonly mode
 * @return HTML
 */
function addressbook_member_view( $mid=0 )  {

 
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook view family member') );

  // Save mid in session scope
  $_SESSION["mid"]=$mid;

  //
  // Fetch data from HTTP request
  //

  // Fetch member from database
  $query  = 'SELECT first_name, middle_name, last_name, birth_day, mobile, email, work, notes,';
  $query .= 'active_roles, wanted_roles, uid, fid, last_updated FROM {addressbook_member} ';
  $query .= 'WHERE mid='.$mid;
  $queryResult = db_query($query);
  $tmp = db_fetch_object($queryResult);

  // If no data is found. Show anti hacking message
  if ($tmp==null) {
    return hacker_warning();
  }

  // Fetch family data from database
  $query2  = 'SELECT street, zipcode, city, country, telephone FROM {addressbook_family} ';
  $query2 .= 'WHERE fid='.$tmp->fid;
  $queryResult2 = db_query($query2);
  $data2 = db_fetch_object($queryResult2);

  // If no data is found. Show anti hacking message
  if ($data2==null) {
    return hacker_warning();
  }

  $name=view_name($tmp->first_name,$tmp->middle_name,$tmp->last_name,false);
  list($birthday_year, $birthday_month, $birthday_day) = split('[/.-]', $tmp->birth_day);
  $telephone=$tmp->telephone;
  $mobile=$tmp->mobile;
  $email=$tmp->email;
  $work=$tmp->work;
  $notes=$tmp->notes;
  $active_roles=$tmp->active_roles;
  $wanted_roles=$tmp->wanted_roles;
  $uid=$tmp->uid;
  $fid=$tmp->fid;
  list($year, $time) = split(' ', $tmp->last_updated);
  list($access_year, $access_month, $access_day) = split('[/.-]', $year);
  list($access_hour, $access_minute, $access_sec) = split(':', $time);
  $last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;

  // View Address Information
  $page .= '<br/>';

  $page .= '<table cellpadding=2 width=100%>';

  // Show this information only to admin users.
  if ( user_access('access administration pages') ) {
     $page .= view_owner($uid,'1');
     $page .= view_line(t('Last updated'),'<input id="text" name="last_updated" size="22" type="text" value="'.$last_updated.'" READONLY />');
  }

  $page .= '<tr><td valign="top" width=15%>';
  $page .= t('Name');
  $page .= '</td><td>';
  $page .= '<input id="text" name="name" size="50" type="text" value="'.$name.'" READONLY />';
  $page .= '</td>';

  // Query Database for Pictures
  $page .= '<td ROWSPAN=9 valign="top">';
  $query  = 'SELECT pid, picture ';
  $query .= 'FROM {addressbook_picture} WHERE fid="'.$fid.'" AND mid="'.$mid.'"';
  $queryResult = db_query($query);
  while ($data = db_fetch_object($queryResult))
  {
    // Show each picture with is found
    $page .= '<a href="'.url(URL_PICTURE_VIEW.'/').$data->pid.'">';
    $filename=url(THUMBNAILS_DIR.'/'.$data->picture);
    $filename=str_replace(array("?q="), "", $filename);
    $page.='<img align="right" src="'.$filename.'"/>';
  }
  $page .= '<tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Street');
  $page .= '</td><td>';
  $page .= '<input id="text" name="street" size="50" maxlength="50" type="text" value="'.$data2->street.'" READONLY />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Zipcode');
  $page .= '</td><td>';
  $page .= '<input id="text" name="zipcode" size="8" maxlength="8" type="text" value="'.$data2->zipcode.'" READONLY />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('City');
  $page .= '</td><td>';
  $page .= '<input id="text" name="city" size="50" maxlength="50" type="text" value="'.$data2->city.'" READONLY />';
  $page .= '</td></tr>';

  $page .= view_country($data2->country, '1');

  $page .= '<tr><td valign="top">';
  $page .= t('Telephone');
  $page .= '</td><td>';
  $page .= '<input id="text" name="telephone" size="16" maxlength="16" type="text" value="'.$data2->telephone.'" READONLY />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top" width=15%>';
  $page .= t('Mobile');
  $page .= '</td><td>';
  $page .= '<input id="text" name="mobile" size="16" type="text" value="'.$mobile.'" READONLY />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top" width=15%>';
  $page .= l(t('Email'),'mailto:'.$email);
  $page .= '</td><td>';
  $page .= '<input id="text" name="email" size="50" type="text" value="'.$email.'" READONLY />';
  $page .= '</td></tr>';

  $page .= view_birthday($birthday_day,$birthday_month,$birthday_year,'1');

  $page .= '<tr><td valign="top" width=15%>';
  $page .= t('Work');
  $page .= '</td><td COLSPAN=2>';
  $page .= '<textarea id="work" cols=55 rows=4 name="work" READONLY >'.$work.'</textarea>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top" width=15%>';
  $page .= t('Notes');
  $page .= '</td><td COLSPAN=2>';
  $page .= '<textarea id="notes" cols=55 rows=4 name="notes" READONLY >'.$notes.'</textarea>';
  $page .= '</td></tr>';
  $page .= '</table>';

  $page .= view_roles($active_roles,$wanted_roles, '1');

  // Show menu Bar
  $page .= '<br/>';
  $page .= '<table align="left">';
  $page .= '<tr>';

  if (variable_get('addressbook_map_link',0)==1) {
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_MAP_VIEW).'" method="POST">';
    $page .= '<input type="hidden" name="street" value="'.$data2->street.'" />';
    $page .= '<input type="hidden" name="city" value="'.$data2->city.'" />';
    $page .= '<input type="hidden" name="country" value="'.$data2->country.'" />';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="hidden" name="mid" value="'.$mid.'" />';
    $page .= '<input type="submit" value="'.t('Go to Map').'" />';
    $page .= '</form>';
    $page .= '</td>';
  }

  $page .= '<td valign="top">';
  if ($_SESSION["list"]=='member') {
     $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  }
  else {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
  }
  $page .= '<input type="submit" name="commit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</td>';


  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<br/>';

  $page .= '</div>';
  print theme("page", $page);
}

function addressbook_email_view( $sort='') 
{
  global $user;
  $page = '<div class="addressbook">';

  $commit = isset($_POST['commit']) ? '1' : '0';

  $subject=$_POST['subject'];
  $body=$_POST['body'];

  $email='';
  $role=$_SESSION["role"];
  $role_type=$_SESSION["role_type"];
  $query  = "SELECT email from {addressbook_member} WHERE last_name LIKE '".$sort."%'";
  if ($_SESSION['role_type']=='active') 
  {
     $query .= 'AND active_roles LIKE "%'.$role.'%" ';
  }
  else 
  {
     $query .= 'AND wanted_roles LIKE "%'.$role.'%" ';
  }
  $queryResult = db_query($query);
  while ($data = db_fetch_object($queryResult)) 
  {
     if ($data->email != '')
     {
       if ($email == '')
       { 
          $email=trim($data->email);
       }
       else
       {
           $email=$email.', '.trim($data->email);
       }
     }
  }
  
  if ($commit=='0')
  {
     $page .= '<table align="left" width="100%">';
     $page .= '<form action="'.url(URL_EMAIL.'/'.$sort).'" method="POST">';
     $page .= '<br/>';
     $page .= t('To:');
     $page .= '<br/>';
     $page .= '<textarea id="email" rows=5 cols=100 maxlength="2000" name="email" WRAP=HARD READONLY>'.$email.'</textarea>';
     $page .= '<br/>';
     $page .= '<br/>';
     $page .= t('Subject:');
     $page .= '<br/>';
     $page .= '<input id="text" name="subject" size=100 maxlength="143" type="text" value="'.$subject.'" />';
     $page .= '<br/>';
     $page .= '<br/>';
     $page .= t('Content:');
     $page .= '<br/>';
     $page .= '<textarea id="body" rows=10 cols=100 maxlength="2000" name="body" >'.$body.'</textarea>';
     $page .= '<br/>';
     $page .= '<br/>';
     $page .= '<table align="left">';
     $page .= '<tr><td>';
     $page .= '<input type="submit" name="commit" value="'.t('Sent').'" />';
     $page .= '</form>';
     $page .= '</td><td>';
     $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
     $page .= '<input type="submit" value="'.t('Cancel').'" />';
     $page .= '</td></tr>';
     $page .= '</form>';
     $page .= '</table>';
     $page .= '</table>';
  }
  else
  {
     $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
     $page .= '<br/>';
    
     if (drupal_mail('Addressbook', 'send', $email, user_preferred_language($user), array('subject'=>$subject,'body'=>$body))) 
     {
        watchdog('Addressbook', t('Sent group email to ').$email);
        $page .= t('Email has be sent to '.$email);
     }
     else 
     {
        watchdog('Addressbook', t('Unable to email to ').$email);
        $page .= t('Unable to email to '.$email);
     }
     $page .= '<br/>';
     $page .= '<br/>';
     $page .= '<input type="submit" name="return" value="'.t('Return').'" />';
  }
  $page .= '</div>';
  print theme("page", $page);
}

/**
 * Render a page showing the selected member in edit mode
 * @return HTML
 */
function addressbook_member_edit( $mid=0 )  {

  global $user;
  $page = '<div class="addressbook">';

  if ($mid==0) {
     drupal_set_title(t('Addressbook create new family member') );
  }
  else {
     drupal_set_title(t('Addressbook edit family member') );
  }

  // Validate HTTP URL parameter
  if ( !check_number($mid) )
  {
     return hacker_warning();
  }

  // Save mid in session scope
  $_SESSION["mid"]=$mid;

  // get fid from session scope
  $fid=$_SESSION["fid"];
  if ($fid=='')
  {
    // If not in session check HTTP header
    $fid=$_POST['fid'];
  }

  // Fetch data from HTTP request
  $commit = isset($_POST['commit']) ? '1' : '0';
  $member_delete = isset($_POST['member_delete']) ? '1' : '0';
  $go_delete = isset($_POST['go_delete']) ? '1' : '0';

  $uid=$user->uid;
  $birthday_day=htmlentities($_POST['birthday_day']);
  $birthday_month=htmlentities($_POST['birthday_month']);
  $birthday_year=htmlentities($_POST['birthday_year']);
  $mobile=htmlentities($_POST['mobile']);
  $email=htmlentities($_POST['email']);
  $work=htmlentities($_POST['work']);
  $notes=htmlentities($_POST['notes']);
  $owner=htmlentities($_POST['owner']);
  $first_name=htmlentities(ucfirst($_POST['first_name']));
  $middle_name=htmlentities($_POST['middle_name']);
  $last_name=htmlentities(ucfirst($_POST['last_name']));

  // Fill in some fields with default values when a new member has been created
  if (($mid=='0') && ($commit=='0')) {
     $query  = 'SELECT middle_name, last_name FROM {addressbook_family} WHERE fid='.$fid;
     $queryResult = db_query($query);
     $tmp = db_fetch_object($queryResult);

     // If no data is found. Show anti hacking message
     if ($tmp==null) {
       return hacker_warning();
     }

     $middle_name=$tmp->middle_name;
     $last_name=$tmp->last_name;

     // If not a new content, check access rights
     $query  = 'SELECT mail FROM {users} WHERE uid='.$user->uid;
     $queryResult = db_query($query);
     $tmp = db_fetch_object($queryResult);
     $email=$tmp->mail;
  }

  /* Fetch all active and wanted roles from the HTTP request */
  $i=0;
  $active_first=1;
  $wanted_first=1;
  $tmp=split( ",", variable_get('addressbook_roles',''));
  while ($tmp[$i]!='') {
    if ($_POST["active_$tmp[$i]"]=='on')
    {
       if ($active_first=='1') {
          $active_roles=$tmp[$i];
          $active_first=0;
       }
       else $active_roles.=';'.$tmp[$i];
    }

    if ($_POST["wanted_$tmp[$i]"]=='on')
    {
       if ($wanted_first=='1') {
          $wanted_roles=$tmp[$i];
          $wanted_first=0;
       }
       else $wanted_roles.=';'.$tmp[$i];
    }
    $i++;
  }

  //
  // Check access rights
  //
  if ( $mid != 0 ) {
     // If not a new content, check access rights
     $query  = 'SELECT uid FROM {addressbook_member} WHERE mid='.$mid;
     $queryResult = db_query($query);
     $tmp = db_fetch_object($queryResult);

     if ( !check_access($tmp->uid)) {
       // Deny access, no more clues.
       // Everything will be in the watchdog's URL for the administrator to check.
       drupal_access_denied();
       return;
     }
  }

  // Delete "Are you sure?" form
  if (($member_delete=='1') && ($mid!='0') && ($go_delete=='0')) {
    $page .= '<br/>';
    $page .= t('Are you sure you want to delete this family member?');
    $page .= '<br/>';
    $page .= '<br/>';
    $page .= '<table align="left">';
    $page .= '<tr><td>';
    $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
    $page .= '<input type="submit" name="go_delete" value="'.t('Yes').'" />';
    $page .= '<input type="hidden" name="member_delete" />';
    $page .= '</form>';
    $page .= '</td><td>';
    $page .= '<form action="'.url(URL_MEMBER_EDIT.'/'.$mid).'" method="POST">';
    $page .= '<input type="submit" name="cancel_delete" value="'.t('No').'" />';
    $page .= '</form>';
    $page .= '</td></tr>';
    $page .= '</table>';
    $page .= '</div>';
    return $page;
  }

  // Delete selected member in database
  if (($commit=='0') && ($member_delete=='1') && ($mid!='0') && ($go_delete=='1')) {
     $query = 'DELETE FROM {addressbook_member} WHERE mid='.$mid;
     $result = db_query($query);
     if ($result==1)
     {
       watchdog('user', 'Address '.$mid.' is deleted in addressbook');
       // Query successfull
       if ($_SESSION["list"]=='member') {
          return member_list($_SESSION["sort"]);
       }
       else {
          return family_view2($fid);
       }
     }
     else {
       // Query failed
       $page .= '<br/>';
       $page .= t('Failed to delete member, try again!');
       $page .= '<br/>';
       $page .= '<br/>';
       if ($_SESSION["list"]=='member') {
          $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
       }
       else {
          $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
       }
       $page .= '<input type="submit" value="'.t('Return').'" />';
       $page .= '</form>';
       $page .= '</div>';
       return $page;
     }
  }

  // Insert new member in database
  if (($commit=='1') && ($member_delete=='0') && ($mid == '0')) {
    $error=0;

    // Valid input.
    if ($first_name=='') $error=1;
    if ($last_name=='') $error=1;
    if (variable_get('addressbook_email_manitory',1)==1) {
      if (check_mail($email)) $error=1;
    }

    if ($error=='0') {
      $birth_day = $birthday_year.'-'.$birthday_month.'-'.$birthday_day;

      if (($owner==0) && !user_access('access administration pages')) {
         // Set new owner, because items did not find owner yet!)
         $owner=$user->uid;
      }

      if (insert_member($first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $fid, $owner)) {
        watchdog('user', 'A address is added in addressbook');
        if ($_SESSION["list"]=='member') {
          return member_list($_SESSION["sort"]);
        }
        else {
          return family_view2($fid);
        }
      }
      else {
        $page .= '<br/>';
        $page .= t('Failed to add member, try again!');
        $page .= '<br/>';
        $page .= '<br/>';
        if ($_SESSION["list"]=='member') {
          $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
        }
        else {
          $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
        }
        $page .= '<input type="submit" value="'.t('Return').'" />';
        $page .= '</form>';
        $page .= '</div>';
        return $page;
      }
    }
  }

  // Update member in database if input in valid
  if (($commit=='1') && ($member_delete=='0') && ($mid != '0')) {
    $error=0;

    // Valid input.
    if ($first_name=='') $error=1;
    if ($last_name=='') $error=1;
    if (variable_get('addressbook_email_manitory',1)==1) {
      if (check_mail($email)) $error=1;
    }

    if ($error=='0') {
      $birth_day = $birthday_year.'-'.$birthday_month.'-'.$birthday_day;

      if (($owner==0) && !user_access('access administration pages')) {
         // Set new owner because items did not found owner yet!)
         $owner=$user->uid;
      }

      if ( update_member($first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $owner, $fid, $mid) ) {
        if ($_SESSION["list"]=='member') {
          return member_list($_SESSION["sort"]);
        }
        else {
          return family_view2($fid);
        }
      }
      else {
        $page .= '<br/>';
        $page .= t('failed to update member, try again!');
        $page .= '<br/>';
        $page .= '<br/>';
        if ($_SESSION["list"]=='member') {
          $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
        }
        else {
          $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
        }
        $page .= '<input type="submit" value="'.t('Return').'" />';
        $page .= '</form>';
        $page .= '</div>';
        return $page;
      }
    }
  }

  // Fetch member from database
  if (($mid!='0') && ($commit=='0')) {
     $query  = 'SELECT first_name, middle_name, last_name, ';
     $query .= 'birth_day, mobile, email, work, notes, active_roles, wanted_roles, uid, fid, last_updated FROM {addressbook_member} WHERE mid='.$mid;
     $queryResult = db_query($query);
     $tmp = db_fetch_object($queryResult);

     // If no data has been found. Show anti hacking message
     if ($tmp==null) {
       return hacker_warning();
     }

     $first_name=$tmp->first_name;
     $middle_name=$tmp->middle_name;
     $last_name=$tmp->last_name;
     list($birthday_year, $birthday_month, $birthday_day) = split('[/.-]', $tmp->birth_day);
     $telephone=$tmp->telephone;
     $mobile=$tmp->mobile;
     $email=$tmp->email;
     $work=$tmp->work;
     $notes=$tmp->notes;
     $active_roles=$tmp->active_roles;
     $wanted_roles=$tmp->wanted_roles;
     $owner=$tmp->uid;
     $fid=$tmp->fid;

     // Save $fid on session scope. Needed for member list edit.
     $_SESSION["fid"]=$fid;

     list($year, $time) = split(' ', $tmp->last_updated);
     list($access_year, $access_month, $access_day) = split('[/.-]', $year);
     list($access_hour, $access_minute, $access_sec) = split(':', $time);
     $last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;
  }

  // View Address Information
  $page .= '<br/>';

  $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '<table border=0 cellpadding=1 width=100%>';

  if ( user_access('access administration pages') ) {
      // Only administrator can overrule owner information.
     $page .= addressbook_view_owner($owner,'0');
  }
  else {
     $page .= addressbook_view_owner($owner,'1');
  }
  $page .= view_line(t('Last updated'),'<input id="text" name="last_updated" size="22" type="text" value="'.$last_updated.'" READONLY />');

  $page .= '<tr><td valign="top" width=15%>';
  $page .= t('First Name*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="first_name" size="35" maxlength="35" type="text" value="'.$first_name.'"';
  if (($first_name=='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td>';

  if ($mid!=0)
  {
    // Query Database for Pictures
    $page .= '<td ROWSPAN=9 valign="top">';
    $query  = 'SELECT pid, picture ';
    $query .= 'FROM {addressbook_picture} WHERE fid="'.$fid.'" AND mid="'.$mid.'"';
    $queryResult = db_query($query);
    $picture_found=0;
    while ($data = db_fetch_object($queryResult))
    {
      $picture_found++;

      // Show each picture which has been found
      $page .= '<a href="'.url(URL_PICTURE_VIEW.'/').$data->pid.'">';
      $filename=url(THUMBNAILS_DIR.'/'.$data->picture);
      $filename=str_replace(array("?q="), "", $filename);
      $page.='<img align="right" src="'.$filename.'"/>';
    }
  }
  $page .= '<tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Middle Name');
  $page .= '</td><td>';
  $page .= '<input id="text" name="middle_name" size="10" maxlength="10" type="text" value="'.$middle_name.'" />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Last Name*');
  $page .= '</td><td>';
  $page .= '<input id="text" name="last_name" size="35" maxlength="35" type="text" value="'.$last_name.'" ';
  if (($last_name='') && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Mobile');
  $page .= '</td><td>';
  $page .= '<input id="text" name="mobile" size="16" maxlength="16" type="text" value="'.$mobile.'" />';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Email');
  if (variable_get('addressbook_email_manitory',1)==1) $page .='*';
  $page .= '</td><td>';
  $page .= '<input id="text" name="email" size="50" maxlength="50" type="text" value="'.$email.'" ';
  if (variable_get('addressbook_email_manitory',1)==1) {
    if (check_mail($email) && ($commit=='1')) $page .= 'class="form-text required error" />'; else $page .= '/>';
  }
  $page .= '</td></tr>';

  $page .= view_birthday($birthday_day,$birthday_month,$birthday_year,'0');

  $page .= '<tr><td valign="top">';
  $page .= t('Work');
  $page .= '</td><td COLSPAN=2>';
  $page .= '<textarea id="work" cols=55 rows=4 maxlength="1000" name="work" >'.$work.'</textarea>';
  $page .= '</td></tr>';

  $page .= '<tr><td valign="top">';
  $page .= t('Notes');
  $page .= '</td><td COLSPAN=2>';
  $page .= '<textarea id="notes" cols=55 rows=4 maxlength="1000" name="notes" >'.$notes.'</textarea>';
  $page .= '</td></tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= t('* Marked fields are mandatory!').'<br/>';

  $page .= view_roles($active_roles, $wanted_roles, '0');

  // Show menu Bar
  $page .= '<br/>';
  $page .= '<table align="left">';
  $page .= '<tr>';

  $page .= '<td valign="top">';
  $page .= '<tr><td valign="top">';
  $page .= '<input type="submit" name="commit" value="'.t('Commit').'" />';
  $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
  $page .= '</form>';
  $page .= '</td>';

  if ($mid!='0') {
    // One picture can only be added to a existing member
    if ($picture_found==0) {
      $page .= '<td valign="top">';
      $page .= '<form action="'.url(URL_PICTURE_EDIT.'/').$mid.'" method="POST">';
      $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
      $page .= '<input type="submit" value="'.t('Add Picture').'" />';
      $page .= '</form>';
      $page .= '</td>';
    }

    // Only existing member can be deleted
    $page .= '<td valign="top">';
    $page .= '<form action="'.url(URL_MEMBER_EDIT.'/').$mid.'" method="POST">';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="submit" name="member_delete" value="'.t('Delete').'" />';
    $page .= '</form>';
    $page .= '</td>';
  }

  // Cancel newly created member
  $page .= '<td valign="top">';
  if ($_SESSION["list"]=='member') {
     $page .= '<form action="'.url(URL_MEMBER_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  }
  else {
     $page .= '<form action="'.url(URL_FAMILY_VIEW.'/').$fid.'" method="POST">';
  }
  $page .= '<input type="submit" value="'.t('Return').'" />';
  $page .= '</form>';
  $page .= '</td>';

  $page .= '</tr>';
  $page .= '</table>';

  $page .= '<br/>';
  $page .= '<br/>';

  $page .= '</div>';
  print theme("page", $page);
}

// ************************************
// Database functions
// ************************************

/*
 * insert member SQL function
 * @return true of false
 */
function addressbook_insert_member( $first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $fid, $owner ) {

  $query  = 'INSERT INTO addressbook_member (first_name, middle_name, last_name, ';
  $query .= 'birth_day, mobile, email, notes, work, active_roles, wanted_roles, uid, fid, last_updated) VALUES (';
  $query .= '"'.$first_name.'",';
  $query .= '"'.$middle_name.'",';
  $query .= '"'.$last_name.'",';
  $query .= '"'.$birth_day.'",';
  $query .= '"'.$mobile.'",';
  $query .= '"'.$email.'",';
  $query .= '"'.$notes.'",';
  $query .= '"'.$work.'",';
  $query .= '"'.$active_roles.'",';
  $query .= '"'.$wanted_roles.'",';
  $query .= '"'.$owner.'",';
  $query .= '"'.$fid.'",';
  $query .= 'SYSDATE() )';

  $result = db_query($query);
  if ($result==1) {
    // Query succesfull
    watchdog('user', 'A member is created in the addressbook');
    return true;
  }
  else {
    // Query failed
    return false;
  }
}


/*
 * update member SQl function
 * @return true of false
 */
function addressbook_update_member($first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $owner, $fid, $mid) {

  $query = 'UPDATE addressbook_member SET ';
  $query .= 'first_name = "'.$first_name.'",';
  $query .= 'middle_name = "'.$middle_name.'",';
  $query .= 'last_name = "'.$last_name.'",';
  $query .= 'birth_day = "'.$birth_day.'",';
  $query .= 'mobile = "'.$mobile.'",';
  $query .= 'email = "'.$email.'",';
  $query .= 'notes = "'.$notes.'",';
  $query .= 'work = "'.$work.'",';
  $query .= 'uid = "'.$owner.'",';
  $query .= 'fid = "'.$fid.'",';
  $query .= 'active_roles = "'.$active_roles.'",';
  $query .= 'wanted_roles = "'.$wanted_roles.'",';
  $query .= 'uid = "'.$owner.'",';
  $query .= 'last_updated = SYSDATE() WHERE mid='.$mid;

  $result = db_query($query);
  if ($result==1) {
    // Query succesfull
    watchdog('user', 'Member '.$mid.' is updated in addressbook');
    return true;
  }
  else {
    // Query failed
    return false;
  }
}
