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
 
function addressbook_family($fid=0) {

	//echo var_dump($_POST);
	
	// *****************************
	// Process POST an SESSION variables
	// *****************************
	
	$Action=htmlentities($_POST['Action']);
	addressbook_debug_param("Action", $Action);
		
	// *****************************
	// Process statemachine
	// *****************************
	
	switch ($Action) {
	
		// ******************
		// Family Actions
		// ******************
   
		case 'FamilyList':
			$page = addressbook_family_list();
			break;
			
		case 'FamilyView':
			$page = addressbook_family_view();
			break;
			
		case 'FamilyEdit':
			$page = addressbook_family_edit();
			break;
			
		case 'FamilySave':
			$error=addressbook_family_save();
			if (!$error) {
				addressbook_save_picture();
				$page .= addressbook_family_view();
			} else {
				$page.= addressbook_family_edit();
			}
			break;
			 
		case 'FamilyDelete':
			addressbook_family_delete();
			addressbook_picture_delete();
			$page = addressbook_family_list();
			break;
			
		// ******************
		// Member Actions
		// ******************
		
		case 'MemberList': 
			$page = addressbook_member_list();
			break;	
							
		case 'MemberEdit': 
			$page = addressbook_member_view(false);
			break;
			
		case 'MemberView': 
			$page = addressbook_member_view(true);
			break;
			
		case 'MemberSave': 
			$error=addressbook_member_save();
			if (!$error) {
				addressbook_save_picture();
				$page = addressbook_family_view();
			} else {
				$page = addressbook_member_view(false);
			}
			break;
			
		case 'MemberDelete': 
			addressbook_delete_member();
			addressbook_picture_delete();
			$page = addressbook_family_view();
			break;
			
		// ******************
		// Other Actions
		// ******************
			
		case 'ShowMap':
			$page = addressbook_map_view();
			break;
			
		case 'Delete':
		    $page = addressbook_delete_view();
			break;
			
		case 'PictureDelete': 
			addressbook_picture_delete();
			$page = addressbook_family_edit();
			break;
			
		default: 
			$page = addressbook_family_list();
			break;
	}
	
	if ($page!="") {
		print theme("page", $page);
	}
}
			
			
// *********************************************************************************
// Views
// *********************************************************************************

/*
 * Function shows all family in a list form
 * @return HTML
 */
function addressbook_family_list() {

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
	addressbook_debug_param("Sort", $Sort);
	$_SESSION["Sort"]=$Sort;

	$Search=$_POST["Search"];
	addressbook_debug_param("Search", $Search);

	// *****************************
	// Query Database
	// *****************************
	
	if ($Search!="") {
		$parameter=$Search;
	} else {
	   $parameter=$Sort;
	}
	
	$query  = '
		SELECT 
			a.fid, 
			a.middle_name, 
			a.last_name, 
			a.street, 
			a.zipcode, 
			a.city, 
			b.picture
		FROM 
			addressbook_family a LEFT JOIN addressbook_picture b ON b.fid=a.fid and b.mid=0
		WHERE 
			LAST_NAME LIKE "'.$parameter.'%" 
		ORDER BY LAST_NAME, MIDDLE_NAME';
  
	$queryResult = db_query($query);

	// *****************************
	// Create page
	// *****************************
	
	drupal_set_title(t('Addressbook family list'));
  
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
  
	// Breadcrumb menu
	$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
	addressbook_breadcrumb($node);

  	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Filteren').'</legend>';
	
	$page .= '<div class="form-item">';
	$page .= '<label>'.t('Achternaam').' :</label>';
	$page .= '<input id="text" name="Search" size="40" maxlength="40" type="text" value="'.$Search.'"> ';
	$page .= addressbook_hiddenlink('AddressbookForm','','','Search',t('Search'));
	$page .= '<div class="description">';
	$page .= t('Enter lastname and press enter to find a family.');
	$page .= '</div>';
	$page .= '</div>';
	
	$page .= addressbook_sortbar($Sort,"List");
	  
	$page .= '</fieldset>';

	$page .= '<table>';
	$page .= '<thead>';
  
	$page .= '<tr>';
  
	if ( variable_get('addressbook_show_thumbnails',0)==1) {
		$page .= '<th>';
      $page .= '<b>'.t('Picture').'<b>';
      $page .= '</th>';
	}
	$page .= '<th>';
	$page .= '<b>'.t('Name').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('Street').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('Zipcode').'</b>';
	$page .= '</th>';
	
	$page .= '<th>';
	$page .= '<b>'.t('City').'</b>';
	$page .= '</th>';
	
	$page .= '</tr>';
	
	$page .= '</thead>';
	$page .= '<tbody>';

	// Show all found families
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {

		if ((++$i%2)==0) {
			$page_tmp .= '<tr class="even">';
		} else {
			$page_tmp .= '<tr class="odd">';
		}
	
		if ( variable_get('addressbook_show_thumbnails',0)==1) {

			$page_tmp .= '<td>';
			$page_tmp .= addressbook_load_picture($data->picture, 50, 60, "right", false);
			$page_tmp .= '</td>';
		}
	
		// Format Name
		$page_tmp .= '<td>';
		if ( variable_get('addressbook_name_format',1)=='1') {
			$name = $data->middle_name.' '.$data->last_name;
		} else {
			$name = $data->last_name.', '.$data->middle_name;
		}
		$page_tmp .= addressbook_hiddenlink('AddressbookForm', 'fid', $data->fid, 'FamilyView', $name);
		$page_tmp .= '</td>';	
	
		$page_tmp .= '<td>';
		$page_tmp .= $data->street;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->zipcode;
		$page_tmp .= '</td>';
		
		$page_tmp .= '<td>';
		$page_tmp .= $data->city;
		$page_tmp .= '</td>';
		
		$page_tmp .= '</tr>';
	}

	if ( $page_tmp!='') {

		// Show Content
		$page .= $page_tmp;
		
	} else {
	
		// No content found
		$page .= '<tr>';
		$page .= '<td>';
		$page .= t('No records found');
		$page .= '</td>';
		$page .= '</tr>';
	}
	$page .= '</tbody>';
	$page .= '</table>';
 
	$page .= addressbook_hiddenlink('AddressbookForm','fid',0,'FamilyEdit',t('New family'));
	$page .= ' | '.l(t("Back"), URL_ADDRESSBOOK);
	
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}
  
/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function addressbook_family_view( $fid = -1)  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	if ($fid==-1) {
		$fid=$_POST["fid"];
		if ($fid=="") {
			$fid=$_SESSION["fid"];
		}
	}
	addressbook_debug_param("fid", $fid);
	
	$_SESSION['fid']=$fid;
		
	// *****************************
	// Query Database
	// *****************************

	$query  = '
		SELECT 
			a.fid, 
			a.middle_name, 
			a.last_name, 
			a.street, 
			a.zipcode, 
			a.city, 
			a.country, 
			a.telephone, 
			a.uid,
			b.picture
		FROM 
			addressbook_family a LEFT JOIN addressbook_picture b ON b.fid=a.fid and b.mid=0
		WHERE 
			a.fid='.$fid;
	
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook family view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
						
	// Breadcrumb menu
	$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
	$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyView', t("Family view"));
	addressbook_breadcrumb($node);

	$page .= '<fieldset>' ;
	
	$page .= addressbook_load_picture($data->picture, 0, 0, "right", true);
	
	$page .= '<h4>'.$data->middle_name.' '.$data->last_name.'</h4>';
	$page .= '<h4>'.$data->street.'</h4>';	
	$page .= '<h4>'.$data->zipcode.'&nbsp;&nbsp;'.$data->city.'</h4>';	
	$page .= '<h4>'.$data->country.'</h4>';	
	$page .= '<h4>'.$data->telephone.'</h4>';	
	
	// Query Family Members
	$query  = '
		SELECT 	
			a.mid, 
			a.first_name, 
			a.middle_name, 
			a.last_name, 
			a.birth_day, 
			a.mobile, 
			a.email, 
			a.uid,
			b.picture as picture
		FROM 
			addressbook_member a LEFT JOIN addressbook_picture b ON b.fid=a.fid and b.mid=a.mid 
		WHERE 
			a.fid='.$fid.' 
		ORDER BY birth_day';
		
	$queryResult = db_query($query);
		
	// Show Banner
	$page .= '<table>';
	$page .= '<thead>';
	$page .= '<tr>';
	$page .= '<th><b>'.t('Phote').'</b></th>';
	$page .= '<th><b>'.t('Name').'</b></th>';	
	$page .= '<th class="active"><b>'.t('Birthday').'</b></th>';
	$page .= '<th><b>'.t('Mobiel').'</b></th>';
	$page .= '<th><b>'.t('Email').'</b></th>';	
	$page .= '</tr>';
	$page .= '</thead>';
	
	// Show all found members
	$page .= '<tbody>';
	$page_tmp='';
	while ($data = db_fetch_object($queryResult)) {

		if ((++$i%2)==0) {
			$page .= '<tr class="even">';
		} else {
			$page .= '<tr class="odd">';
		}

		if ( variable_get('addressbook_show_thumbnails',0)==1 ) {
			$page .= '<td>';
			$page .= addressbook_load_picture($data->picture, 25, 30, "left", false);
			$page .= '</td>';
		}	
	
		$page .= '<td>';
		$name=addressbook_view_name($data->first_name,$data->middle_name,$data->last_name,true);
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$data->mid,'MemberView',$name);
		$page .= '</td>';
		
		$page .= '<td>';
		list($year, $month, $day) = split('[/.-]', $data->birth_day);
		$page .= $day.'-'.$month.'-'.$year;
		$page .= '</td>';

		$page .= '<td>';
		$page .= $data->mobile;
		$page .= '</td>';
		
		$page .= '<td>';
		$page .= l($data->email,'mailto:'.$data->email);
		$page .= '</td>';
		
		$page .= '</tr>';
	}

	$page .= '</tbody>';
	$page .= '</table>';
	
	if (addressbook_check_access($uid)) {
  
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyEdit',t('Family Edit'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid', $fid, 'MemberEdit', t('Member Add'));
			
		if (variable_get('addressbook_map_link',0)==1) {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'ShowMap',t('Show Map'));
		}
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','','','FamilyList',t('Return'));
	} else { 
		$page .= addressbook_hiddenlink('AddressbookForm','','','FamilyList',t('Return'));	
	}
	
	
	$page .= '</div>';
	$page .= '</form>';
	
	return $page;
}

/**
 * Render a page showing the selected family in detail
 * @return HTML
 */
function addressbook_family_edit()  {
		
	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	addressbook_debug_param("fid", $fid);
	
	// *****************************
	// Query Database
	// *****************************
	
	$query = '
		SELECT 
			a.fid, 
			a.middle_name, 
			a.last_name, 
			a.street, 
			a.zipcode, 
			a.city, 
			a.country, 
			a.telephone, 
			a.last_updated, 
			a.uid,
			b.picture
		FROM 
			addressbook_family a LEFT JOIN addressbook_picture b ON b.fid=a.fid and b.mid=0
		WHERE 
			a.fid='.$fid;
					
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Check Access
	// *****************************
	
	if ( !addressbook_check_access($data->uid) ) {
			drupal_access_denied();
			return;
	}
	
	// *****************************
	// Get posted Inputs
	// *****************************
	
	if ( isset($_POST["middle_name"]) ) {
		$middle_name=htmlentities($_POST['middle_name']);
	} else {
	   $middle_name=$data->middle_name;
	}
	
	if ( isset($_POST["last_name"]) ) {
		$last_name=htmlentities(ucfirst($_POST['last_name']));
	} else {
		$last_name=$data->last_name;
	}
	
	if ( isset($_POST["street"]) ) {
		$street=htmlentities(ucfirst($_POST['street']));
	} else {
		$street=$data->street;
	}
	
	if ( isset($_POST["zipcode"]) ) {
		$zipcode=htmlentities($_POST['zipcode']);
	} else {
		$zipcode=$data->zipcode;
	}
	
	if ( isset($_POST["city"]) ) {
		$city=htmlentities($_POST['city']);
	} else {
		$city=$data->city;
	}
	
	if ( isset($_POST["country"]) ) {
		$country=htmlentities($_POST['country']);
	} else {
		$country=$data->country;
	}
	if ($fid==0) {
		$country=variable_get('addressbook_country', 'Nederland');
	}
	
	if ( isset($_POST["telephone"]) ) {
		$telephone=htmlentities($_POST['telephone']);
	} else {
		$telephone=$data->telephone;
	}
	
	if ( isset($_POST["owner"]) ) {
		$owner=htmlentities($_POST['owner']);
	} else {
		$owner=$data->owner;
	}
			
	// *****************************
	// Create Page
	// *****************************

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';

	drupal_set_title(t('Addressbook family edit'));

	// Breadcrumb menu
	$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
	$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyView', t("Family view"));
	addressbook_breadcrumb($node);
	
	$page .= '<fieldset>' ;	
	$page .= '<legend>'.t('Family details').'</legend>';
	
	$page .= addressbook_load_picture($data->picture, 0, 0, "right", true);
	
	$page .= addressbook_form_item(
		t("Last Access"),
		addressbook_view_timestamp($data->last_updated),
		t("Last access date"), 
		true );
		
	$page .= addressbook_form_item(
		t("Owner"),
		addressbook_view_owner($owner,$readonly),
		t("Drupal owner (user) of address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Middle Name"),
		addressbook_view_text("middle_name", 10, 10, $middle_name, $readonly),
		t("Middle name of the family name (for example: van, de, etc..)"), 
		$readonly );

	$page .= addressbook_form_item(
		t("Last Name").addressbook_view_manitory(),
		addressbook_view_text("last_name", 35, 35, $last_name, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Street").addressbook_view_manitory(),
		addressbook_view_text("street", 50, 50, $street, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Zipcode"),
		addressbook_view_text("zipcode", 8, 8, $zipcode, $readonly),
		t("Zipcode of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("City").addressbook_view_manitory(),
		addressbook_view_text("city", 50, 50, $city, $readonly),
		t("City of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Country").addressbook_view_manitory(),
		addressbook_view_country($country, $readonly),
		t("Country of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Telephone"),
		addressbook_view_text("telephone", 16, 16, $telephone, $readonly),
		t("Telephone of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Image file"),
		addressbook_input_picture($readonly),
		t("Only jpg format is supported"), 
		$readonly );
				
	// Menu bar
	
	$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilySave',t('Save'));
	if (addressbook_check_picture()) {
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'PictureDelete',t('Delete Picture'));
	}
	if ($fid!=0) {
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'Delete',t('Delete Family'));
	}
	$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Cancel'));

	$page .= '</fieldset>';	
	
	$page .= '</div>';
	$page .= '</form>';
	
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
		$mid=0;
	}
	addressbook_debug_param("mid", $mid);
	
	$fid=$_POST["fid"];
	if ($fid=="") {
		$fid=$_SESSION['fid'];
	}
	addressbook_debug_param("fid", $fid);

	// *****************************
	// Check Access
	// *****************************
	
	if ( !readonly && !addressbook_check_access($data->uid) ) {
		drupal_access_denied();
		return;
	}
	
	// *****************************
	// Query Database
	// *****************************
	
	// Fetch member from database
	if ($mid==0) {
	
		$query  = '
			SELECT 
				b.street as street, 
				b.zipcode as zipcode, 
				b.city as city, 
				b.country as country, 
				b.telephone as telephone,
				b.last_name as last_name,
				b.middle_name as middle_name
			FROM 
				addressbook_family b
			WHERE 
				b.fid='.$fid;
			
	} else {
			
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
				b.telephone as telephone,
				c.picture as picture
			FROM 
				addressbook_member a, addressbook_family b LEFT JOIN addressbook_picture c ON c.mid='.$mid.' 
			WHERE 
				b.fid=a.fid and
				a.mid='.$mid;
	}
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Get posted Inputs
	// *****************************
	
	if ( isset($_POST["first_name"]) ) {
		$first_name=htmlentities($_POST['first_name']);
	} else {
		$first_name=$data->first_name;
	}
	addressbook_debug_param("first_name",$first_name);

	if ( isset($_POST["last_name"]) ) {
		$last_name=htmlentities($_POST['last_name']);
	} else {
		$last_name=$data->last_name;
	}
	addressbook_debug_param("last_name",$last_name);
	
	if ( isset($_POST["birth_day"]) ) {
		$birth_day=htmlentities($_POST['birth_day']);
	} else {
		$birth_day=$data->birth_day;
	}
	addressbook_debug_param("birth_day",$birth_day);
	
  	
	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook member view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
		
	// Breadcrumb menu
	$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
	$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyView', t("Family view"));
	$node["child3"] = addressbook_hiddenlink('AddressbookForm', 'mid', $mid, 'MemberView', t("Member view"));
	addressbook_breadcrumb($node);
		
	$page .= '<fieldset>' ;

	$page .= addressbook_load_picture($data->picture, 0, 0, "right", true);
	
	$page .= '<h4>'.$data->street.'</h4>';	
	$page .= '<h4>'.$data->zipcode.'&nbsp;&nbsp;'.$data->city.'</h4>';	
	$page .= '<h4>'.$data->country.'</h4>';	
	$page .= '<h4>'.$data->phone.'</h4>';	

	$page .= '</fieldset>' ;
		
	$page .= '<fieldset>' ;

	// Show this information only to admin users.
	if ( user_access('access administration pages') ) {
		
		if ($data->last_updated!="") {
			list($year, $time) = split(' ', $data->last_updated);
			list($access_year, $access_month, $access_day) = split('-', $year);
			list($access_hour, $access_minute, $access_sec) = split(':', $time);
			$last_updated=$access_day.'-'.$access_month.'-'.$access_year.' '.$access_hour.':'.$access_minute.':'.$access_sec;

			$page .= addressbook_form_item(
				t("Last updated:"),
				addressbook_view_text("last_updated", 22, 22, $last_updated, true),
				"", 
				true );
		}
		
		$page .= addressbook_form_item(
			t("Member Id:"),
			$mid,
			t("Member id"), 
			true );
			
		$page .= addressbook_form_item(
			t("Owner:"),
			addressbook_view_owner($data->uid, $readonly),
			t("Drupal owner of this member"), 
			$readonly );
	}

	
	$page .= addressbook_form_item(
					t("First Name:").addressbook_view_manitory(),
					addressbook_view_text("first_name", 50, 50, $first_name, $readonly),
					t("First Name of member"), 
					$readonly );
				
	$page .= addressbook_form_item(
					t("Middle Name:"),
					addressbook_view_text("middle_name", 10, 10, $data->middle_name, $readonly),
					t("Middle Name of member"), 
					$readonly );
				
	$page .= addressbook_form_item(
					t("Last Name:".addressbook_view_manitory()),
					addressbook_view_text("last_name", 50, 50, $last_name, $readonly),
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
					addressbook_view_textarea('work', 80, 4, 512, $data->work, $readonly),
					t("Work notes"), 
					$readonly );
					
	$page .= addressbook_form_item(
					t("Notes:"),
					addressbook_view_textarea('notes', 80, 4, 512, $data->notes, $readonly),
					t("General notes"), 
					$readonly );

	if (!addressbook_check_picture())  {
		$page .= addressbook_form_item(
					t("Image file"),
					addressbook_input_picture($readonly),
					t("Only jpg format is supported"), 
					$readonly );
	}
		
	$page .= addressbook_view_roles($data->active_roles,$data->wanted_roles, $readonly);

	$page .= '</fieldset>' ;

	// *****************************
	// Create Menu Bar
	// *****************************

	if ($readonly) {
		if (addressbook_check_access($uid)) {
			$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberEdit',t('Edit'));		
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Back'));
		} else {
			$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Back'));
		}
	} else {
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberSave',t('Save'));
		if ($mid!=0) {
			if (addressbook_check_picture()) {
				$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'PictureDelete',t('Delete Picture'));
			}
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberDelete',t('Delete Member'));
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberView',t('Cancel'));
		} else {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Cancel'));
		}
	}
		
	
	$page .= '</div>';
	$page .= '</form>';
 
	return $page;
}

function addressbook_map_view() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=$_POST["fid"];
	addressbook_debug_param("fid", $fid);
	
	// *****************************
	// Query Database
	// *****************************
	
	$query  = '
			SELECT 
				b.last_name as last_name,
				b.middle_name as middle_name,
				b.street as street, 
				b.city as city, 
				b.country as country
			FROM 
				addressbook_family b
			WHERE 
				b.fid='.$fid;
		
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);
	
	// *****************************
	// Create page
	// *****************************
  
	drupal_set_html_head('<script type=\'text/javascript\' src=\'http://api.map24.com/ajax/1.2/?init=default\'></script>');
	drupal_set_html_head('<script type=\'text/javascript\'>');
	drupal_set_html_head('var map = null;');

	drupal_set_html_head('function goMap24() {');
	drupal_set_html_head('var mrcContainer = new Map24.Webservices.Request.MapletRemoteControl(map);');

	drupal_set_html_head('map = Map24.Webservices.getMap24Application({');
	drupal_set_html_head('AppKey: "'.variable_get('addressbook_map_key', '').'",');
	drupal_set_html_head('MapArea: document.getElementById( "maparea" ),');
	drupal_set_html_head('Maptype: "JAVA",');
	drupal_set_html_head('MapWidth: 640,');
	drupal_set_html_head('MapHeight: 480,');
	drupal_set_html_head('MRC: mrcContainer');
	drupal_set_html_head('});');
	drupal_set_html_head('geocode(\''.$data->street.','.$data->city.','.$data->country.'\')');
	drupal_set_html_head('}');

	drupal_set_html_head('function geocode( addressString ){');
	drupal_set_html_head('map.Webservices.sendRequest(');
	drupal_set_html_head('new Map24.Webservices.Request.MapSearchFree(map, {');
	drupal_set_html_head('SearchText: addressString,');
	drupal_set_html_head('MaxNoOfAlternatives: 50');
	drupal_set_html_head('})');
	drupal_set_html_head(');');

	drupal_set_html_head('map.onMapSearchFree = function( event ){');
	drupal_set_html_head('var mrcContainer = new Map24.Webservices.Request.MapletRemoteControl( map );');
	drupal_set_html_head('var firstResult = event.Alternatives[0];');
	drupal_set_html_head('var lon = firstResult.Coordinate.Longitude;');
	drupal_set_html_head('var lat = firstResult.Coordinate.Latitude;');

	drupal_set_html_head('mrcContainer.push(');
	drupal_set_html_head('new Map24.Webservices.MRC.SetMapView({');
	drupal_set_html_head('Coordinates: new Map24.Coordinate( lon, lat ),');
	drupal_set_html_head('ClippingWidth: new Map24.Webservices.ClippingWidth(');
	drupal_set_html_head('{ MinimumWidth: 250 }');
	drupal_set_html_head(')');
	drupal_set_html_head('})');
	drupal_set_html_head(');');
	drupal_set_html_head('map.Webservices.sendRequest( mrcContainer );');
	drupal_set_html_head('}');
	drupal_set_html_head('}');

	drupal_set_html_head('</script>');

	$page.='<body onload=\'goMap24()\'>';

	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
	
	drupal_set_title(t('Addressbook map view'));
		
		// Breadcrumb menu
	$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
	$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyView', t("Family view"));
	$node["child3"] = t("Map view");
	addressbook_breadcrumb($node);

	$page .= '<field>';
	
	$page .= '<br/>';
	$page .= '<h4>'.$data->middle_name.' '.$data->last_name.'</h4>';
	$page .= '<h4>'.$data->street.'</h4>';
	$page .= '<h4>'.$data->city.'</h4>';
	$page .= '<h4>'.$data->country.'</h4>';
	$page .= '<br/>';
	
	$page .= '<div id="maparea"></div>';
		
	$page .= '</field>';
	$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Back'));
		
	$page .= '</div>';
	$page .= '</form>';
	$page .= '</body>';
		
	return $page;
}

function addressbook_delete_view() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$mid=$_POST["mid"];
	if ($mid=="") {
		$mid=0;
	}
	addressbook_debug_param("mid", $mid);
	
	$fid=$_POST["fid"];
	if ($fid=="") {
		$fid=$_SESSION['fid'];
	}
	addressbook_debug_param("fid", $fid);
	
	// *****************************
	// Create views
	// *****************************
	
	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';
	
	$page .= '<fieldset>';
	
	$page .= '<h4>';
	if (($mid!=0) && ($fid!=0)) {
	
		// Breadcrumb menu
		$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
		$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyEdit', t("Family view"));
		$node["child3"] = addressbook_hiddenlink('AddressbookForm', 'mid', $mid, 'MemberEdit', t("Member view"));
		addressbook_breadcrumb($node);

		$page .= t("Delete selected member. Are you sure?");
		
	} else {
		// Breadcrumb menu
		$node["child1"] = l(t('Family list'),URL_ADDRESSBOOK_FAMILY);
		$node["child2"] = addressbook_hiddenlink('AddressbookForm', 'fid', $fid, 'FamilyEdit', t("Family view"));
		addressbook_breadcrumb($node);

		$page .= t("Delete selected family. Are you sure?");
	}
	$page .= '</h4>';
	$page .= t("<br/>");
	
	if (($fid!=0) && ($mid!=0)) {
		$page .= addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberDelete',t('Yes'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','mid',$mid,'MemberEdit',t('No'));
	} else {
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyDelete',t('Yes'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyEdit',t('No'));
	}

	$page .= '</fieldset>' ;

	$page .= '</div>';
	$page .= '</form>'; 
 
	return $page;
}

// *********************************************************************************
// Database functions
// *********************************************************************************

/*
 * insert family SQL function
 * @return true of false
 */
function addressbook_family_save() {

	// *****************************
	// Get posted Inputs
	// *****************************

	$fid=htmlentities($_POST["fid"]);
	if ($fid=="") {
		$fid=0;
	}
	addressbook_debug_param("fid", $fid);
		
	$middle_name=htmlentities($_POST['middle_name']);
	addressbook_debug_param("middle_name",$middle_name);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	addressbook_debug_param("last_name",$last_name);
	
	$street=htmlentities(ucfirst($_POST['street']));
	addressbook_debug_param("street",$street);
	
	$zipcode=htmlentities($_POST['zipcode']);
	addressbook_debug_param("zipcode",$zipcode);
	
	$city=htmlentities(ucfirst($_POST['city']));
	addressbook_debug_param("city",$city);
	
	$country=htmlentities($_POST['country']);
	addressbook_debug_param("country",$country);
	
	$telephone=htmlentities($_POST['telephone']);
	addressbook_debug_param("telephone",$telephone);
	
	$owner=htmlentities($_POST['owner']);
	addressbook_debug_param("owner",$owner);
	
	// *****************************
	// Input Validation
	// *****************************
	
	$error=0;

	if ($last_name=="") {

		$msg = t('Last name is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}

	if ($street=="") {

		$msg = t('Street is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}
	
	if ($city=="") {

		$msg = t('City is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}
	
	if ($country=="") {

		$msg = t('Country is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}
	
	if ($error==1) {
		return true;
	}

	// *****************************
	// Update Database
	// *****************************

	if ($fid==0) {
	
		// Check if family all exisit, reload button protection
		$query = 'select fid from addressbook_family where 
					zipcode like "'.$zipcode.'" and 
					street like "'.$street.'"';
			
		$queryResult = db_query($query);
		$data = db_fetch_object($queryResult);
		if ($data->fid!="") {

			// Reload detect
			return true;
		}
		
		$query  = 'INSERT INTO addressbook_family (middle_name, last_name, street, zipcode, city, ';
		$query .= 'country, telephone, uid, last_updated) VALUES (';
		$query .= '"'.$middle_name.'",';
		$query .= '"'.$last_name.'",';
		$query .= '"'.$street.'",';
		$query .= '"'.$zipcode.'",';
		$query .= '"'.$city.'",';
		$query .= '"'.$country.'",';
		$query .= '"'.$telephone.'",';
		$query .= '"'.$owner.'",';
		$query .= 'SYSDATE() )';

		db_query($query);
	
		// Check new family entry.
		$query = 'select fid from addressbook_family where 
					zipcode like "'.$zipcode.'" and 
					street like "'.$street.'"';
			
		$queryResult = db_query($query);
		$data = db_fetch_object($queryResult);
			
		$_SESSION['fid']=$data->fid;
		
		// *****************************
		// create message
		// *****************************
	
		$msg .= t('Family').' '.$data->fid.' '.t('created!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
	} else {
		
		// *****************************
		// Update Database
		// *****************************
	
		$query = 'UPDATE addressbook_family SET ';
		$query .= 'middle_name = "'.$middle_name.'",';
		$query .= 'last_name = "'.$last_name.'",';
		$query .= 'street = "'.$street.'",';
		$query .= 'zipcode = "'.$zipcode.'",';
		$query .= 'city = "'.$city.'",';
		$query .= 'country = "'.$country.'",';
		$query .= 'telephone = "'.$telephone.'",';
		$query .= 'uid = "'.$owner.'",';
		$query .= 'last_updated = SYSDATE() ';
		$query .= 'WHERE fid='.$fid;

		db_query($query);
	
		// *****************************
		// create message
		// *****************************
	
		$msg .= t('Family').' '. $fid.' '.t('updated!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
	}
	return false;
}


/*
 * delelete family SQL function
 * @return true of false
 */
function addressbook_family_delete() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	
	// *****************************
	// Database actions
	// *****************************

	if ($fid!=0) {
	
		$query = 'delete FROM addressbook_family WHERE fid='.$fid;		
		db_query($query);  

		$query = 'delete FROM addressbook_member WHERE fid='.$fid;	
		db_query($query);  
  
		$query = 'SELECT picture FROM addressbook_picture WHERE mid=0 and fid='.$fid;
		$queryResult = db_query($query);
		while ($data = db_fetch_object($queryResult)) {
			unlink(ADDRESSBOOK_IMAGE_DIR.'/'.$data->picture);
			unlink(ADDRESSBOOK_THUMBNAILS_DIR.'/'.$data->picture);
		}
	
		$query = 'delete FROM addressbook_picture WHERE fid='.$fid;	
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Family').' '.$fid.' '.t('deleted!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	
	return 1;		
}

/*
 * Save member data to database
 *
 * @return error (true of false)
 */
function addressbook_member_save() {

	// *****************************
	// Get posted Inputs
	// *****************************

	$mid=htmlentities($_POST["mid"]);
	addressbook_debug_param("mid", $mid);
	
	$fid=$_SESSION["fid"];
	addressbook_debug_param("fid", $fid);
	
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
	
	/* Fetch all active and wanted roles from the HTTP request */
	$i=0;
	$active_first=1;
	$wanted_first=1;
	$tmp=split( ",", variable_get('addressbook_roles',''));
	while ($tmp[$i]!='') {
		if ($_POST["active_$tmp[$i]"]=='on') {
			if ($active_first=='1') {
				$active_roles=$tmp[$i];
				$active_first=0;
			} else {
				$active_roles.=';'.$tmp[$i];
			}
		}
    

		if ($_POST["wanted_$tmp[$i]"]=='on') {
			if ($wanted_first=='1') {
				$wanted_roles=$tmp[$i];
				$wanted_first=0;
			} else {
				$wanted_roles.=';'.$tmp[$i];
			}
		}
		$i++;
	}
	addressbook_debug_param("active_roles", $active_roles);
	addressbook_debug_param("wanted_roles", $wanted_roles);

	// *****************************
	// Input Validation
	// *****************************
	
	$error=0;
	
	if ($first_name=="") {

		$msg = t('First name is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}

	if ($last_name=="") {

		$msg = t('Last name is manitory');
		drupal_set_message( $msg, 'error');
		$error=1;
	}

	if ($error==1) {
		return true;
	}
	
	// *****************************
	// Update Database
	// *****************************
	
	if ($mid==0) {
		
		// Check if user allready exist. Protect against reload button
		$query  = 'select ';
		$query .= ' a.uid as uid ';
		$query .= 'from ';
		$query .= ' addressbook_member a ';
		$query .= 'where ' ;
		$query .= ' a.first_name="'.$first_name.'" and ';
		$query .= ' a.middle_name="'.$middle_name.'" and ';
		$query .= ' a.last_name="'.$last_name.'"';
	
		addressbook_debug_sql($query);
		$queryResult = db_query($query);     
		$data = db_fetch_object($queryResult);
	
		if ($data->uid!="") {
		
			// reload detect, return direct!
			return true;
		}
				
		// Insert new member 
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
				fid,
				last_updated) VALUES (';
		$query .= '"'.$first_name.'",';
		$query .= '"'.$middle_name.'",';
		$query .= '"'.$last_name.'",';
		$query .= '"'.addressbook_convert_date_reverse($birthday).'",';
		$query .= '"'.$mobile.'",';
		$query .= '"'.$email.'",';
		$query .= '"'.$notes.'",';
		$query .= '"'.$work.'",';
		$query .= '"'.$active_roles.'",';
		$query .= '"'.$wanted_roles.'",';
		$query .= '"'.$owner.'",';
		$query .= '"'.$fid.'",';
		$query .= 'SYSDATE() )';

		db_query($query);
		
		// Get new Member id
		$query  = 'select ';
		$query .= ' a.uid as uid ';
		$query .= 'from ';
		$query .= ' addressbook_member a ';
		$query .= 'where ' ;
		$query .= ' a.first_name="'.$first_name.'" and ';
		$query .= ' a.middle_name="'.$middle_name.'" and ';
		$query .= ' a.last_name="'.$last_name.'"';
	
		addressbook_debug_sql($query);
		$queryResult = db_query($query);     
		$data = db_fetch_object($queryResult);
			
		// *****************************
		// create message
		// *****************************
	
		$msg .= t('Member').' '.$data->mid.' '.t('created');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);	
	
	} else {
		
		$query = 'UPDATE addressbook_member SET ';
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
	
		$msg .= t('Member').' '.$mid.' '.t('updated');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);	
	}
	return false;
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
	addressbook_debug_param("mid", $mid);
	
	// *****************************
	// Database actions
	// *****************************

	if ($mid!=0) {
	
		// Delete member
		$query = 'delete FROM addressbook_member WHERE mid='.$mid;		
		db_query($query);  
	
		// *****************************
		// Create message
		// *****************************
	
		$msg .= t('Member').' '.$mid.' '.t('is deleted.');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	}
	return 1;	
}


function addressbook_picture_delete() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=$_POST["fid"];
	if ($fid=="") {
		$fid=0;
	}
	addressbook_debug_param("fid",$fid);
  
	// get fid from session scope
	$mid=$_POST["mid"];
	if ($mid=="") {
		$mid=0;
	}
	addressbook_debug_param("mid",$mid);

	if ($fid!=0) {
		$query = 'SELECT picture FROM addressbook_picture WHERE fid='.$fid;
	} else {
		$query = 'SELECT picture FROM addressbook_picture WHERE mid='.$mid;
	}
	
	// In old addressbook version more image could be connected to one family.
	// therefor we must delete all related image files.
	$queryResult = db_query($query);
	while ($data = db_fetch_object($queryResult)) {
		unlink(ADDRESSBOOK_IMAGE_DIR.'/'.$data->picture);
		unlink(ADDRESSBOOK_THUMBNAILS_DIR.'/'.$data->picture);
	}
	
	if ($fid!=0) {
		$query = 'delete FROM addressbook_picture WHERE fid='.$fid;	
	} else { 
		$query = 'delete FROM addressbook_picture WHERE mid='.$mid;
	}
	db_query($query);  
	
	// *****************************
	// Create message
	// *****************************
	
	$msg .= t('Picture is deleted!');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
}

// *********************************************************************************
// The end
// *********************************************************************************
