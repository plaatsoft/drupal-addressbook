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
   			
		case 'MemberInsert': 
			$page = addressbook_member_view();
			break;
			
		case 'MemberUpdate': 
			$page = addressbook_member_view();
			break;
		
		case 'MemberDelete': 
			$page = addressbook_member_view();
			break;
			
		case 'MemberView': 
			$page = addressbook_member_view();
			break;
			
		case 'MemberList': 
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

	drupal_set_title(t('Addressbook member list'));
  
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
  
  	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Filteren').'</legend>';
	
	$page .= '<div class="form-item">';
	$page .= '<label>'.t('Achternaam').' :</label>';
	$page .= '<input id="text" name="Search" size="40" maxlength="40" type="text" value="'.$Search.'"> ';
	$page .= addressbook_hiddenlink('AddressbookForm','','','Search',t('Search'));
	$page .= '<div class="description">';
	$page .= t('Enter lastname and press enter to find the member.');
	$page .= '</div>';
	$page .= '</div>';
	
	$page .= addressbook_sortbar($Sort,"List");
	
	$page .= '</fieldset>';
 
	$page .= '<table>';
	$page .= '<thead>';

	$page .= '<tr>';
	if (user_access('access administration pages')) {
		$page .= '<th>';
		$page .= t('Id');
		$page .= '</th>';
	}
		
	if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
		$page .= '<th>';
		$page .= '<b>'.t('Picture').'</b>';
		$page .= '</th>';
	}
	$page .= '<th>';
	$page .= '<b>'.t('Name').'</b>';
	$page .= '</th>';
	$page .= '<th>';
	$page .= '<b>'.t('Telephone').'</b>';
	$page .= '</th>';
	$page .= '<th>';
	$page .= '<b>'.t('Mobile').'</b>';
	$page .= '</th>';
	$page .= '<th>';
	$page .= '<b>'.t('Email').'</b>';
	$page .= '</th>';
	$page .= '</tr>';
	
	$page .= '</thead>';
	$page .= '<tbody>';
	
	// Show all found members
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {
	
		if ((++$i%2)==0) {
			$page_tmp .= '<tr class="even">';
		} else {
			$page_tmp .= '<tr class="odd">';
		}
		
		if (user_access('access administration pages')) {
			$page_tmp .= '<td>';
			$page_tmp .= $data->mid;
			$page_tmp .= '</td>';
		}

		if ( variable_get('addressbook_show_thumbnails',0)==1) {
			$query2  = 'SELECT pid, picture ';
			$query2 .= 'FROM {addressbook_picture} ';
			$query2 .= 'WHERE fid="'.$data->fid.'" AND mid="'.$data->mid.'"';
			$queryResult2 = db_query($query2);
			$tmp = db_fetch_object($queryResult2);
		
			$page_tmp .= '<td>';
			if ($tmp!=null) {
				// Show each picture with is found
				if ( check_access($data->uid) ) {
					$page_tmp .= '<a href="'.url(URL_MEMBER_EDIT.'/').$data->mid.'">';
				} else {
					$page_tmp .= '<a href="'.url(URL_MEMBER_VIEW.'/').$data->mid.'">';
				}
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page_tmp .='<img src="'.$filename.'" width=50 height=60/></a>';
			} 
			$page_tmp .= '</td>';
		}
     
		// Format Name
		$page_tmp .= '<td>';
		$name=view_name($data->first_name,$data->middle_name,$data->last_name,true);
		$page_tmp .= oostpoort_hiddenlink('AddressbookForm', 'mid', $data->mid, 'MemberView', $name);
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->telephone;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->mobile;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= l($data->email,'mailto:'.$data->email);
		$page_tmp .= '</td>';
		
		$page_tmp .= '</tr>';
	}

	if ( $page_tmp!='') {
     $page .= $page_tmp;
	} else {
		// No content found
		$page .= '<tr>'; 
		$page .= t('No records found');
		$page .= '</tr>';
  }
  $page .= '</table>';
  $page .= '</div>';
  
  return $page;
}

/**
 * Render a page showing the selected member in readonly mode
 * @return HTML
 */
function addressbook_member_view(  )  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=$_POST["mid"];
	if ($mid=="") {
		$mid=$_SESSION["mid"];
	}
	
	// *****************************
	// Query Database
	// *****************************
	
	// Fetch member from database
	$query  = '
		SELECT 
			a.first_name as first_name, 
			a.middle_name as middle_name, 
			a.last_name as last_name, 
			a.birth_day as birth_day, 
			a.mobile as mobile, 
			a.email as email, 
			a.work as work, 
			a.notes as notes,
			a.active_roles as active_roles, 
			a.wanted_roles as wanted_roles, 
			a.uid as uid, 
			a.fid as fid, 
			a.last_updated as last_updated
			b.street as street, 
			b.zipcode as zipcode, 
			b.city as city, 
			b.country as country, 
			b.telephone as telephone
		FROM 
			addressbook_member a, addressbook_family b
		WHERE 
			b.fid=a.fid and
			mid='.$mid;
		
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);
  
	// *****************************
	// Create page
	// *****************************

	$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,false);
	list($birthday_year, $birthday_month, $birthday_day) = split('-', $data->birth_day);
	list($year, $time) = split(' ', $data->last_updated);
	list($access_year, $access_month, $access_day) = split('-', $year);
	list($access_hour, $access_minute, $access_sec) = split(':', $time);
	$last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.
							$access_hour.':'.$access_minute.':'.$access_sec;

	drupal_set_title(t('Addressbook member view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	//addressbook_save_picture("fid",$fid);
						
	$page .= '<fieldset>' ;
	$page .= '<legend></legend>';

	// Show this information only to admin users.
	if ( user_access('access administration pages') ) {
		
		$page .= addressbook_form_item(
		t("Owner"),
		addressbook_view_owner($uid, $readonly),
		t("Owner of this member"), 
		$readonly );
		
		$page .= addressbook_form_item(
		t("Last updated"),
		addressbook_view_text("middle_name", 22, 22, $last_updated, true),
		"", 
		true );
	}

	$page .= addressbook_form_item(
					t("Name"),
					addressbook_view_text("name", 50, 50, $data->name, $readonly),
					t("Member name"), 
					$readonly );
		
	// Query Database for Pictures
	$page .= '<td ROWSPAN=9 valign="top">';
	$query  = 'SELECT pid, picture ';
	$query .= 'FROM {addressbook_picture} WHERE fid="'.$fid.'" AND mid="'.$mid.'"';
	$queryResult = db_query($query);
	while ($data = db_fetch_object($queryResult)) {
		// Show each picture with is found
		$page .= '<a href="'.url(URL_PICTURE_VIEW.'/').$data->pid.'">';
		$filename=url(THUMBNAILS_DIR.'/'.$data->picture);
		$filename=str_replace(array("?q="), "", $filename);
		$page.='<img align="right" src="'.$filename.'"/>';
	}
	$page .= '<tr>';

	$page .= addressbook_form_item(
					t("Street"),
					addressbook_view_text("street", 50, 50, $data->street, $readonly),
					t("Street name"), 
					$readonly );
		
	$page .= addressbook_form_item(
					t("Zipcode"),
					addressbook_view_text("zipcode", 8, 8, $data->zipcode, $readonly),
					t("Zipcode of member"), 
					$readonly );

	$page .= addressbook_form_item(
					t("Zipcode"),
					addressbook_view_text("city", 50, 50, $data->city, $readonly),
					t("City of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Country"),
					addressbook_view_country($data->country, $readonly),
					t("Country of member"), 
					$readonly );

	$page .= addressbook_form_item(
					t("Telephone"),
					addressbook_view_text("telephone", 16, 16, $data->telephone, $readonly),
					t("Telephone number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Mobile"),
					addressbook_view_text("mobile", 16, 16, $data->mobile, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Email"),
					addressbook_view_text("email", 50, 50, $data->email, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Birthday"),
					addressbook_view_birthday($birthday_day,$birthday_month,$birthday_year, $readonly),
					t("Birtday of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Work"),
					addressbook_view_textarea('work', 55, 4, 512, $data->work, $readonly),
					t("Work notes"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Notes"),
					addressbook_view_textarea('notes', 55, 4, 512, $data->notes, $readonly),
					t("General notes"), 
					$readonly );

	$page .= addressbook_view_roles($active_roles,$wanted_roles, '1');

	$page .= '</fieldset>' ;

  // Show menu Bar
  // Menu bar
	if ($mid==0) {
		$page .= addressbook_hiddenlink('AddressbookForm','',0,'MemberInsert',t('Save'));
	} else {
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'MemberUpdate',t('Save'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'MemberDelete',t('Delete'));
	}
	$page .= ' | '.addressbook_hiddenlink('AddressbookForm','',0,'MemberList',t('Cancel'));
	
	
 
   /*if (variable_get('addressbook_map_link',0)==1) {
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
  }*/



	$page .= '</div>';
	$page .= '</form>';
 
	return $page;
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
