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
			addressbook_insert_member();
			$page = addressbook_member_view(true);
			break;
			
		case 'MemberUpdate': 
			addressbook_update_member();
			$page = addressbook_member_view(true);
			break;
		
		case 'MemberDelete': 
			addressbook_delete_member();
			$page = addressbook_member_list();
			break;
		
		case 'MemberEdit': 
			$page = addressbook_member_view(false);
			break;
			
		case 'MemberView': 
			$page = addressbook_member_view(true);
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
				if ( addressbook_check_access($data->uid) ) {
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
		$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,true);
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
function addressbook_member_view( $readonly=true )  {

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
			a.last_updated as last_updated,
			b.street as street, 
			b.zipcode as zipcode, 
			b.city as city, 
			b.country as country, 
			b.telephone as telephone
		FROM 
			addressbook_member a, addressbook_family b
		WHERE 
			b.fid=a.fid and
			a.mid='.$mid;
		
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);
  
	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook member view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	//addressbook_save_picture("fid",$fid);
						
	$page .= '<fieldset>' ;

	// Show this information only to admin users.
	if ( user_access('access administration pages') ) {
		
		list($year, $time) = split(' ', $data->last_updated);
		list($access_year, $access_month, $access_day) = split('-', $year);
		list($access_hour, $access_minute, $access_sec) = split(':', $time);
		$last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;

		$page .= addressbook_form_item(
			t("Last updated:"),
			addressbook_view_text("last_updated", 22, 22, $last_updated, true),
			"", 
			true );
		
		$page .= addressbook_form_item(
			t("Owner:"),
			addressbook_view_owner($data->uid, $readonly),
			t("Drupal owner of this member"), 
			$readonly );
	}

	// Query Database for Pictures
	/*$page .= '<td ROWSPAN=9 valign="top">';
	$query  = 'SELECT pid, picture ';
	$query .= 'FROM {addressbook_picture} WHERE fid="'.$data->fid.'" AND mid="'.$mid.'"';
	$queryResult = db_query($query);
	while ($data2 = db_fetch_object($queryResult)) {
		// Show each picture with is found
		$page .= '<a href="'.url(URL_PICTURE_VIEW.'/').$data2->pid.'">';
		$filename=url(THUMBNAILS_DIR.'/'.$data2->picture);
		$filename=str_replace(array("?q="), "", $filename);
		$page.='<img align="right" src="'.$filename.'"/>';
	}
	$page .= '</td>';*/
	
	$page .= addressbook_form_item(
					t("First Name:"),
					addressbook_view_text("first_name", 50, 50, $data->first_name, $readonly),
					t("First Name of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Middle Name:"),
					addressbook_view_text("middle_name", 10, 10, $data->middle_name, $readonly),
					t("Middle Name of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Last Name:"),
					addressbook_view_text("last_name", 50, 50, $data->last_name, $readonly),
					t("Last Name of member"), 
					$readonly );
						
	$page .= addressbook_form_item(
					t("Mobile:"),
					addressbook_view_text("mobile", 16, 16, $data->mobile, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Email:"),
					addressbook_view_text("email", 50, 50, $data->email, $readonly),
					t("Mobile number of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Birthday:"),
					addressbook_view_birthday($data->birth_day, $readonly),
					t("Birtday of member"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Work:"),
					addressbook_view_textarea('work', 55, 4, 512, $data->work, $readonly),
					t("Work notes"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Notes:"),
					addressbook_view_textarea('notes', 55, 4, 512, $data->notes, $readonly),
					t("General notes"), 
					$readonly );

	$page .= addressbook_view_roles($active_roles,$wanted_roles, '1');

	$page .= '</fieldset>' ;

	// *****************************
	// Create Menu Bar
	// *****************************

	if ($readonly) {
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberEdit',t('Edit'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberList',t('Cancel'));
	} else {
		if ($mid==0) {
			$page .= addressbook_hiddenlink('AddressbookForm','mid',0,'MemberInsert',t('Save'));
		} else {
			$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberUpdate',t('Save'));
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberDelete',t('Delete'));
		}
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberView',t('Cancel'));
	}
	
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
function addressbook_insert_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=htmlentities($_POST["mid"]);
	
	$first_name=htmlentities($_POST['first_name']);
	
	$middle_name=htmlentities($_POST['middle_name']);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	
	$mobile=htmlentities($_POST['mobile']);
	
	$email=htmlentities($_POST['email']);
	
	$notes=htmlentities($_POST['notes']);
	
	$work=htmlentities($_POST['work']);
	
	$notes=htmlentities($_POST['notes']);
	
	$owner=htmlentities($_POST['owner']);
	
	$birthday=$_POST['birthday_day'].'-'.$_POST['birthday_month'].'-'.$_POST['birthday_year'];
	
	// *****************************
	// Update Database
	// *****************************
	
  $query  = '
				INSERT INTO addressbook_member (
				first_name, 
				middle_name, 
				last_name, 
				birth_day, 
				mobile, 
				email, 
				notes, 
				work, 
				active_roles, 
				wanted_roles, 
				uid, 
				last_updated) VALUES (';
  $query .= '"'.$first_name.'",';
  $query .= '"'.$middle_name.'",';
  $query .= '"'.$last_name.'",';
  $query .= '"'.addressbook_convert_date_reverse($birth_day).'",';
  $query .= '"'.$mobile.'",';
  $query .= '"'.$email.'",';
  $query .= '"'.$notes.'",';
  $query .= '"'.$work.'",';
  $query .= '"'.$active_roles.'",';
  $query .= '"'.$wanted_roles.'",';
  $query .= '"'.$owner.'",';
  $query .= 'SYSDATE() )';

	db_query($query);

	// *****************************
	// create message
	// *****************************
	
	$msg .= t('Member').' '.$mid.' '.t('is created in addressbook');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
}

/*
 * update member SQl function
 * @return true of false
 */
function addressbook_update_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=htmlentities($_POST["mid"]);
	addressbook_debug_param("mid", $mid);
	
	$first_name=htmlentities($_POST['first_name']);
	addressbook_debug_param("first_name", $first_name);
	
	$middle_name=htmlentities($_POST['middle_name']);
	addressbook_debug_param("middle_name", $middle_name);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	addressbook_debug_param("last_name", $last_name);
	
	$mobile=htmlentities($_POST['mobile']);
	addressbook_debug_param("mobile", $mobile);
	
	$email=htmlentities($_POST['email']);
	addressbook_debug_param("email", $email);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$work=htmlentities($_POST['work']);
	addressbook_debug_param("work", $work);
	
	$notes=htmlentities($_POST['notes']);
	addressbook_debug_param("notes", $notes);
	
	$owner=htmlentities($_POST['owner']);
	addressbook_debug_param("owner", $owner);
	
	$birthday=$_POST['birthday_day'].'-'.$_POST['birthday_month'].'-'.$_POST['birthday_year'];
	addressbook_debug_param("birthday", $birthday);
	
	// *****************************
	// Update Database
	// *****************************
	
	$query = 'UPDATE {addressbook_member} SET ';
	$query .= 'first_name = "'.$first_name.'",';
	$query .= 'middle_name = "'.$middle_name.'",';
	$query .= 'last_name = "'.$last_name.'",';
	$query .= 'birth_day = "'.addressbook_convert_date_reverse($birthday).'",';
	$query .= 'mobile = "'.$mobile.'",';
	$query .= 'email = "'.$email.'",';
	$query .= 'notes = "'.$notes.'",';
	$query .= 'work = "'.$work.'",';
	$query .= 'uid = "'.$owner.'",';
	$query .= 'active_roles = "'.$active_roles.'",';
	$query .= 'wanted_roles = "'.$wanted_roles.'",';
	$query .= 'uid = "'.$owner.'",';
	$query .= 'last_updated = SYSDATE() WHERE mid='.$mid;

	db_query($query);
	addressbook_debug_sql($query);

	// *****************************
	// create message
	// *****************************
	
	$msg .= t('Member').' '.$mid.' '.t('is updated in addressbook');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
}


/*
 * Delete member SQl function
 * @return true of false
 */
function addressbook_delete_member() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=htmlentities($_POST["mid"]);
	
	// *****************************
	// Database actions
	// *****************************

	if ($mid!=0) {
	
		$query = 'delete FROM addressbook_member WHERE mid='.$mid;		
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Member').' '.$mid.' '.t('deleted!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	
	return 1;	
}

// *********************************************************************************
// The end
// *********************************************************************************
