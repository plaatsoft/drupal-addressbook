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

/**
 * Render a page for the csv addressbook upload
 * @return HTML
 */
 
 function addressbook_upload_csv() {

	//only administrators can access this module
	if (!user_access('access administration pages')) {
		return drupal_access_denied();
	}
  		
	// Breadcrumb menu
	$node["child1"] = t('Addressbook csv upload');
	$page .= addressbook_breadcrumb($node);
	
	drupal_set_title(t('Addressbook upload'));
	
	$page .= '<form enctype="multipart/form-data" name="OostpoortForm" method="POST" >';
	$page .= '<div class="addressbook">';
	
	$page .= addressbook_save_csv();
		
	$page .= '<fieldset>' ;
	$page .= '<legend>'.t('Addressbook file upload').'</legend>';	
	$page .= '<div class="form-item">';
	$page .= '  <label>'.t('File').':</label>';
	$page .= '  <input name="uploadedfile" type="file" size="100" onChange="submit();" />';
	$page .= '  <div class="description">';
	$page .= t('Alleen csv formaat wordt ondersteund en het bestand mag niet groter zijn dan ').ini_get('upload_max_filesize').'B';
	$page .= '  </div>';
	$page .= '</div>';
	$page .= '</fieldset>' ;  
	
	$page .= '<a href="'.url(URL_ADDRESSBOOK).'">'.t('Terug').'</a>';
	
	$page .= '</div>';
	$page .= '</form>';  
		
	print theme("page", $page);
}

function addressbook_save_csv() {

	// Fetch Image upload in a HTTP request
	if ($_FILES['uploadedfile']['tmp_name']!='') 
	{
		if ( (strstr($_FILES['uploadedfile']['name'],'.csv')) || 
		     (strstr( $_FILES['uploadedfile']['name'],'.CSV')) ) {
	
			$page .= addressbook_process_csv($_FILES['uploadedfile']['tmp_name']);
			
		} else {
				
			$msg = t("Only CSV file format is supported.");
			drupal_set_message($msg, 'error');
	
			$_FILES['uploadedfile']['name']='';
		}
	}
	return $page;
}
 
/**
 * process csv upload file.
 * @return errors in HTML format
 */
function addressbook_process_csv($filename) {

  global $user;

  $line_counter=0;
  $csv_file_error=0;

  $fh = fopen($filename, 'r');

  // Header line, skip it!
  $theData = fgets($fh);
  $line_counter++;

  $theData = fgets($fh);
  $line_counter++;
  while ($theData!='')
  {
    $line_error=0;

	// *******************************
    // Conver data
    // *******************************
	
	$value=split( ',' ,$theData);
    
	$data["first_name"]=ucfirst($value[0]);
    
	$data["middle_name"]=$value[1];
    
	$data["last_name"]=ucfirst($value[2]);
    
	$data["street"]=ucfirst($value[3]);
    
	$data["zipcode"]=$value[4];
    
	$data["city"]=ucfirst($value[5]);
    
	$data["country"]=$value[6];
    
	$data["telephone"]=$value[7];
	
    $data["mobile"]=$value[8];
	
    $data["email"]=$value[9];
	
    list($day, $month, $year) = split('[/.-]', $value[10]);
    $data["birth_day"]=$year.'-'.$month.'-'.$day;

    $tmp=str_replace(array("~~"), "\r\n", $value[11]);
    $data["work"]=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[12]);
    $data["notes"]=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[13]);
    $data["active_roles"]=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[14]);
    $data["wanted_roles"]=$tmp;

	// *******************************
    // Validate data
    // *******************************
	
    if ($data["first_name"]=='') {
		$msg = t('Line').' ['.$line_counter.'] '.t('First name empty');
		drupal_set_message($msg, 'error');
		$line_error=1;
    }
	
    if ($data["last_name"]=='') {
		$msg = t('Line').' ['.$line_counter.'] '.t('Last name empty');
		drupal_set_message($msg, 'error');
		$line_error=1;
    }
	
	if ($data["street"]=='') {
		$msg = t('Line').' ['.$line_counter.'] '.t('street empty');
		drupal_set_message($msg, 'error');
		$line_error=1;
    }
	
	if ($data["zipcode"]=='') {
		$msg = t('Line').' ['.$line_counter.'] '.t('zipcode empty');
		drupal_set_message($msg, 'error');
		$line_error=1;
    }
	
	if ($data["city"]=='') {
		$msg = t('Line').' ['.$line_counter.'] '.t('city empty');
		drupal_set_message($msg, 'error');
		$line_error=1;
    }
	
	if ($line_error==1) {
		// Error, skip line!
		continue;
	}

	// *******************************
    // Update database
    // *******************************
	
    addressbook_family_csv_update($data);
	addressbook_member_csv_update($data);     
	
	// *******************************
    // Get next line.
    // *******************************
	
	$line_counter++;    
    $theData = fgets($fh);
  }
  fclose($fh);

  // Delete csv file
  unlink($myFile);
}


function addressbook_family_csv_update($data) {
	
	// *****************************
	// Update Database
	// *****************************

	// Check if family all exist
	$query = '
		select 
			fid 
		from 
			addressbook_family 
		where 
			zipcode like "'.$data["zipcode"].'" and 
			street like "'.$data["street"].'"';
			
	$queryResult = db_query($query);
	$data = db_fetch_object($queryResult);
	$fid=$data->fid;

	if ($fid=="") {
		
		$query  = 'INSERT INTO addressbook_family (middle_name, last_name, street, zipcode, city, ';
		$query .= 'country, telephone, uid, last_updated) VALUES (';
		$query .= '"'.$data["middle_name"].'",';
		$query .= '"'.$data["last_name"].'",';
		$query .= '"'.$data["street"].'",';
		$query .= '"'.$data["zipcode"].'",';
		$query .= '"'.$data["city"].'",';
		$query .= '"'.$data["country"].'",';
		$query .= '"'.$data["telephone"].'",';
		$query .= '"'.$data["owner"].'",';
		$query .= 'SYSDATE() )';

		db_query($query);
	
		// Check new family entry.
		$query = 'select fid from addressbook_family where 
					zipcode like "'.$data["zipcode"].'" and 
					street like "'.$data["street"].'"';
			
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
		$query .= 'middle_name = "'.$data["middle_name"].'",';
		$query .= 'last_name = "'.$data["last_name"].'",';
		$query .= 'street = "'.$data["street"].'",';
		$query .= 'zipcode = "'.$data["zipcode"].'",';
		$query .= 'city = "'.$data["city"].'",';
		$query .= 'country = "'.$data["country"].'",';
		$query .= 'telephone = "'.$data["telephone"].'",';
		$query .= 'uid = "'.$data["owner"].'",';
		$query .= 'last_updated = SYSDATE() ';
		$query .= 'WHERE fid='.$data["fid"];

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