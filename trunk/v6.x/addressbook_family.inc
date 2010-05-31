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
 
function addressbook_family($PrsId=0) {

	//echo var_dump($_POST);
	
	// *****************************
	// Process POST an SESSION variables
	// *****************************
	
	$Action=htmlentities($_POST['Action']);
	
	// *****************************
	// Process statemachine
	// *****************************
	
	switch ($Action) {
   
		case 'FamilyList':
			$page = addressbook_family_list();
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
			$page = addressbook_family_list();
			break;
	}
	print theme("page", $page);
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
	
	$query  = '
		SELECT 
			fid, 
			middle_name, 
			last_name, 
			street, 
			zipcode, 
			city, 
			uid
		FROM 
			{addressbook_family} 
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

			// Query sort found pictures on birtday
			$query2 = 'SELECT picture FROM {addressbook_picture} WHERE fid="'.$data->fid.'" AND mid="0"';
			$queryResult2 = db_query($query2);
			if ($tmp = db_fetch_object($queryResult2)) {
				// Show first picture with is found
				$page_tmp .= '<a href="'.url(URL_FAMILY_VIEW.'/'.$data->fid).'">';
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page_tmp .='<img src="'.$filename.'" width=60 height=50/></a>';
			}	
			
			$page_tmp .= '</td>';
		}
	
		// Format Name
		$page_tmp .= '<td>';
		if ( variable_get('addressbook_name_format',1)=='1') {
			$name = $data->middle_name.' '.$data->last_name;
		} else {
			$name = $data->last_name.', '.$data->middle_name;
		}
		$page_tmp .= oostpoort_hiddenlink('AddressbookForm', 'fid', $data->fid, 'FamilyView', $name);
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
function addressbook_family_view( )  {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=$_POST["fid"];
	if ($fid=="") {
		$fid=$_SESSION["fid"];
	}
	
	// *****************************
	// Query Database
	// *****************************

	$query  = '
		SELECT 
			fid, 
			middle_name, 
			last_name, 
			street, 
			zipcode, 
			city, 
			country, 
			telephone, 
			uid 
		FROM 
			{addressbook_family} 
		WHERE 
			FID='.$fid;
			
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

	// *****************************
	// Create page
	// *****************************

	drupal_set_title(t('Addressbook family view') );

	$page .= '<form enctype="multipart/form-data" name="AddressbookForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	//addressbook_save_picture("fid",$fid);
						
	$page .= '<fieldset>' ;
	$page .= '<legend></legend>';

	//$page .= addressbook_picture("Adr",$fid, 0, 0,"right", true);
	
	$page .= '<h4>'.$data->middle_name.' '.$data->last_name.'</h4>';
	$page .= '<h4>'.$data->street.'</h4>';	
	$page .= '<h4>'.$data->zipcode.'&nbsp;&nbsp;'.$data->city.'</h4>';	
	$page .= '<h4>'.$data->country.'</h4>';	
	$page .= '<h4>'.$data->telephone.'</h4>';	
	
	// Query Family Members
	$query  = '
		SELECT 	
			mid, 
			first_name, 
			middle_name, 
			last_name, 
			birth_day, 
			mobile, 
			email, 
			uid 
		FROM 
			{addressbook_member} 
		WHERE 
			fid='.$fid.' 
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
			$query2  = 'SELECT pid, picture FROM {addressbook_picture} WHERE fid="'.$fid.'" AND mid="'.$data->mid.'"';
			$queryResult2 = db_query($query2);
			$tmp = db_fetch_object($queryResult2);

			$page .= '<td>';
			
			if ($tmp!=null) {	
				// Show each picture with is found
				$filename=url(THUMBNAILS_DIR.'/'.$tmp->picture);
				$filename=str_replace(array("?q="), "", $filename);
				$page .= '<img src="'.$filename.'" width=25 height=30/>';
			} 
			$page .= '</td>';
      }
	
		$page .= '<td>';
		$name=view_name($data->first_name,$data->middle_name,$data->last_name,true);
		if ( check_access( $data->uid )) {
			$page .= l($name,URL_MEMBER_EDIT.'/'.$data->mid);
		} else {
			$page .= l($name,URL_MEMBER_VIEW.'/'.$data->mid);
		}
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
	
	if (check_access($uid)) {
  
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyEdit',t('Family Edit'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'MemberAdd',t('Member Add'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyPicture',t('Picture Add'));
	
		if (variable_get('addressbook_map_link',0)==1) {
			$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'Map',t('Show Map'));
		}
	}
	
	$page .= ' | '.oostpoort_hiddenlink('AddressbookForm','','','FamilyList',t('Return'));
	
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
	
	// *****************************
	// Query Database
	// *****************************
	
	$query = '
		SELECT 
			fid, 
			middle_name, 
			last_name, 
			street, 
			zipcode, 
			city, 
			country, 
			telephone, 
			last_updated, 
			uid 
		FROM 
			{addressbook_family} 
		WHERE 
			fid='.$fid;
			
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);

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

	$page .= '<form name="AddressbookForm" method="POST" >';	
	$page .= '<div class="addressbook">';

	drupal_set_title(t('Addressbook family edit'));
	
	// Check access rights
	if ( $fid != 0 ) {
	
		// If not a new family is created, check entry access rights
		$query  = 'SELECT uid FROM {addressbook_family} WHERE fid='.$fid;
		$queryResult = db_query($query);
		$tmp = db_fetch_object($queryResult);

		if ( !check_access($tmp->uid) ) {
			// Deny access, no more clues.
			// Everything will be in the watchdog's URL for the administrator to check.
			drupal_access_denied();
			return;
		}
	}	

	$page .= '<fieldset>' ;	
	$page .= '<legend>'.t('Family details').'</legend>';
	
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
		t("Last Name"),
		addressbook_view_text("last_name", 35, 35, $last_name, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Street"),
		addressbook_view_text("street", 50, 50, $street, $readonly),
		t("Last name of the family name."), 
		$readonly );

	$page .= addressbook_form_item(
		t("Zipcode"),
		addressbook_view_text("zipcode", 8, 8, $zipcode, $readonly),
		t("Zipcode of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("City"),
		addressbook_view_text("city", 50, 50, $city, $readonly),
		t("City of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Country"),
		addressbook_view_country($country, $readonly),
		t("Country of family address."), 
		$readonly );
		
	$page .= addressbook_form_item(
		t("Telephone"),
		addressbook_view_text("telephone", 16, 16, $telephone, $readonly),
		t("Telephone of family address."), 
		$readonly );

	// Menu bar
	if ($fid==0) {
		$page .= addressbook_hiddenlink('AddressbookForm','',0,'FamilyInsert',t('Save'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','',0,'FamilyList',t('Cancel'));
	} else {
		$page .= addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyUpdate',t('Save'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyDelete',t('Delete'));
		$page .= ' | '.addressbook_hiddenlink('AddressbookForm','fid',$fid,'FamilyView',t('Cancel'));
	}
	
	
	$page .= '</fieldset>';	
	
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
function addressbook_family_insert() {

	// *****************************
	// Get posted Inputs
	// *****************************
		
	$middle_name=htmlentities($_POST['middle_name']);
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	$street=htmlentities(ucfirst($_POST['street']));
	$zipcode=htmlentities($_POST['zipcode']);
	$city=htmlentities(ucfirst($_POST['city']));
	$country=htmlentities($_POST['country']);
	$telephone=htmlentities($_POST['telephone']);
	$owner=htmlentities($_POST['owner']);
	
	// *****************************
	// Update Database
	// *****************************

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
	$query = '
	    select 
			fid 
		from 
			{addressbook_family} 
		where 
			zipcode like "'.$zipcode.'" and 
			street like "'.$street.'"';
			
	$queryResult = db_query($query);
	$tmp = db_fetch_object($queryResult);
	$fid = $tmp->fid;
	
	$_SESSION['fid']=$fid;
		
	// *****************************
	// create message
	// *****************************
	
	if ($fid!=0) {
		$msg .= t('Family').' '.$fid.' '.t('created!');
		drupal_set_message($msg, 'status');
		watchdog('user', $msg);
		
		return 0;
	} 
		
	return 1;
}

/*
 * update family SQL function
 * @return true of false
 */
function addressbook_family_update() {

	// *****************************
	// Get posted Inputs
	// *****************************
	
	$fid=htmlentities($_POST["fid"]);
	
	$middle_name=htmlentities($_POST['middle_name']);
	
	$last_name=htmlentities(ucfirst($_POST['last_name']));
	
	$street=htmlentities(ucfirst($_POST['street']));
	
	$zipcode=htmlentities($_POST['zipcode']);
	
	$city=htmlentities(ucfirst($_POST['city']));
	
	$country=htmlentities($_POST['country']);
	if ($country=='') {
		$country=variable_get('addressbook_country', 'Nederland');
	}

	$telephone=htmlentities($_POST['telephone']);

	$owner=htmlentities($_POST['owner']);
	
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

	$result = db_query($query);
	
	// *****************************
	// create message
	// *****************************
	
	$msg .= t('Family').' '.$fid.' '.t('updated!');
	drupal_set_message($msg, 'status');
	watchdog('user', $msg);
	
	return 0;
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

// *********************************************************************************
// The end
// *********************************************************************************