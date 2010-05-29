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

  global $user;
  $page = '<div class="addressbook">';

  drupal_set_title(t('Addressbook csv upload') );

  //only administrators can access this module
  if (!user_access('access administration pages')) {
    return drupal_access_denied();;
  }

   // Fetch Image upload in a HTTP request
  if ($_FILES['uploadedfile']['name']!='') {
    if ( (!strstr($_FILES['uploadedfile']['name'],'.csv')) && (!strstr($_FILES['uploadedfile']['name'],'.CSV')) )
    {
      $page .= '<br/>';
      $page.=t('Only CSV format is supported').'<br/>';
      $page .= '<br/>';

      // Return to family view page
      $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
      $page .= '<input type="submit" value="'.t('Cancel').'" />';
      $page .= '</form>';

      $page .= '</div>';
      return $page;
    }
    else
    {
      if (move_uploaded_file($_FILES['uploadedfile']['tmp_name'],file_directory_path().'/upload.csv')) {
        watchdog('user', 'A csv file is uploaded to the addressbook');
        $page .= process_csv_upload();
      }
      else {
        // Query Failed
        $page .= '<br/>';
        $page .= t('Failed to save csv file, please try again!').'<br/>';
        $page .= '<br/>';

        // Return to family view page
        $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
        $page .= '<input type="submit" value="'.t('Cancel').'" />'.'<br/>';
        $page .= '</form>';

        $page .= '</div>';
        return $page;
      }
    }
  }

  if ($_FILES['uploadedfile']['name']=='') {
    $page .= '<form enctype="multipart/form-data" action="'.url(URL_UPLOAD_CSV).'" method="POST">';
    $page .= '<br/>';
    $page .= '<input type="hidden" />';
    $page .= t('Choose a csv file to upload:').' <input name="uploadedfile" type="file" />';

    $page .= '<br/>';
    $page .= '<div id="mission">';
    $page .= t('Warning: Only csv formatted files are supported.');
    $page .= '<br/>';
    $page .= t('File size must not exceed').ini_get('upload_max_filesize').'B';
    $page .= '<br/></div>';
    $page .= '<br/>';

    // Show menu Bar
    $page .= '<table>';
    $page .= '<tr><td>';
    $page .= '<input type="hidden" name="fid" value="'.$fid.'" />';
    $page .= '<input type="submit" value="'.t('Upload File').'" />';
    $page .= '</form>';
    $page .= '</td><td>';

    // Return to family view page
    $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
    $page .= '<input type="submit" value="'.t('Cancel').'" />';
    $page .= '</form>';

    $page .= '</td></tr>';
    $page .= '</table>';
  }

  $page .= '</div>';
  print theme("page", $page);
}

 
/**
 * process csv upload file.
 * @return errors in HTML format
 */
function process_csv_upload() {

  global $user;

  $page = '<div class="addressbook">';
  $page .= '<br/>';

  $line_counter=0;
  $csv_file_error=0;

  $myFile = file_directory_path().'/upload.csv';
  $fh = fopen($myFile, 'r');

  // Header line, skip it!
  $theData = fgets($fh);
  $line_counter++;

  $theData = fgets($fh);
  $line_counter++;
  while ($theData!='')
  {
    $line_error=0;

    // Split read line in elements
    $value=split( ',' ,$theData);
    $first_name=ucfirst($value[0]);
    $middle_name=$value[1];
    $last_name=ucfirst($value[2]);
    $street=ucfirst($value[3]);
    $zipcode=$value[4];
    $city=ucfirst($value[5]);
    $country=$value[6];
    $telephone=$value[7];
    $mobile=$value[8];
    $email=$value[9];
    list($day, $month, $year) = split('[/.-]', $value[10]);
    $birth_day=$year.'-'.$month.'-'.$day;

    $tmp=str_replace(array("~~"), "\r\n", $value[11]);
    $work=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[12]);
    $notes=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[13]);
    $active_roles=$tmp;

    $tmp=str_replace(array("~~"), "\r\n", $value[14]);
    $wanted_roles=$tmp;

    // Validate input. If error don't insert entry in database
    if ($first_name=='') {
       $page.=t('Line').' ['.$line_counter.'] '.t('First name empty').'<br/>';
       $line_error=1;
    }
    if ($last_name=='') {
       $page.=t('Line').' ['.$line_counter.'] '.t('Last name empty').'<br/>';
       $line_error=1;
    }
    if ($street=='') {
       $page.=t('Line').' ['.$line_counter.'] '.t('Street empty').'<br/>';
       $line_error=1;
    }
    if ($zipcode=='') {
       $page.=t('Line').' ['.$line_counter.'] '.t('Zipcode empty').'<br/>';
       $line_error=1;
    }
    if ($city=='') {
       $page.=t('Line').' ['.$line_counter.'] '.t('City empty').'<br/>';
       $line_error=1;
    }

    if (variable_get('addressbook_email_manitory',1)==1) {
      if (check_mail($email)) {
         $page.=t('Line').' ['.$line_counter.'] '.t('Email not valid').'['.$email.']<br/>';
         $line_error=1;
      }
    }

    // Show warning if validate fails
    if ($line_error==1) {
      $page.=t('Line').' ['.$line_counter.'] '.t('contains an error, entry is skipped').'<br/>';
    }
    else
    {
      // Input is ok. Insert / Update database with information

      // Check if family already exist, if not create new family.
      $query='SELECT fid, uid FROM {addressbook_family} WHERE street="'.$street.'" AND city="'.$city.'" AND country="'.$country.'"';
      $queryResult = db_query($query);
      $tmp = db_fetch_object($queryResult);

      if ($tmp!=null){
        // Update Family
        $fid=$tmp->fid;
        if (!update_family($middle_name, $last_name, $street, $zipcode, $city, $country, $telephone, $tmp->uid, $fid))
        {
           // Query failed
           $page.=t('Line ').'['.$line_counter.'] '.t('contains an error (Family update failed)').'<br/>';
           $line_error=1;
        }
      }
      else {
        // Insert Family
        if (!insert_family($middle_name, $last_name, $street, $zipcode, $city, $country, $telephone, 0))
        {
          // Query failed
          $page.=t('Line ').'['.$line_counter.'] '.t('contains an error (Family insert failed)').'<br/>';
          $line_error=1;
        }
        else
        {
          // Insert succesfull, get Family ID
          $query='SELECT fid FROM {addressbook_family} WHERE middle_name="'.$middle_name.'" AND last_name="'
            .$last_name.'" AND street="'.$street.'" AND city="'.$city.'"';
          $queryResult = db_query($query);
          $tmp = db_fetch_object($queryResult);
          $fid=$tmp->fid;
        }
      }

      // If family insert/update was successful, then continue with member insert/update.
      if ( $line_error==0 )
      {
        // Check if member already exist
        $query='SELECT mid, uid FROM {addressbook_member} WHERE first_name="'.$first_name.'" AND middle_name="'
          .$middle_name.'" AND last_name="'.$last_name.'"';
        $queryResult = db_query($query);
        $tmp = db_fetch_object($queryResult);
        if ($tmp!=null){
          // Member existed
          $mid=$tmp->mid;
          if (!update_member($first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $tmp->uid, $fid, $mid)) {
            // Query failed
            $page.=t('Line ').'['.$line_counter.'] '.t('contains a error (Member update failed)').'<br/>';
            $line_error=1;
          }
        }
        else {
          if (!insert_member( $first_name, $middle_name, $last_name, $birth_day, $mobile, $email, $notes, $work, $active_roles, $wanted_roles, $fid, 0))
          {
            // Query failed
            $page.=t('Line ').'['.$line_counter.'] '.t('contains a error (Member insert failed)').'<br/>';
            $line_error=1;
          }
        }
      }
    }
    $line_counter++;
    $theData = fgets($fh);
  }
  fclose($fh);

  // Delete csv file
  unlink($myFile);

  $page .= '<br/>';
  $page .= t('CSV file processed!').'<br/>';
  $page .= '<br/>';
  $page .= '<form action="'.url(URL_FAMILY_LIST.'/'.$_SESSION["sort"]).'" method="POST">';
  $page .= '<input type="submit" value="'.t('Return').'" />';
  $page .= '</form>';

  $page .= '</div>';
  return $page;
}




