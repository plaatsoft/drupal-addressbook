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
 * Create CSV file and start file transfer to web user.
 */
function addressbook_download_csv() {

  $myFile = file_directory_path().'/addressbook.csv';

  drupal_set_title(t('Addressbook csv download'));

  //only addressbook administrators can access the functionality
  if (!user_access('administer addressbook')) {
    return drupal_access_denied();;
  }

  $fp = fopen( $myFile, 'w');

  $query  = 'SELECT first_name, middle_name, last_name, mobile, email, birth_day, work, notes, active_roles, wanted_roles, ';
  $query .= '(SELECT street FROM {addressbook_family} WHERE fid=addressbook_member.fid) street, ';
  $query .= '(SELECT zipcode FROM {addressbook_family} WHERE fid=addressbook_member.fid) zipcode, ';
  $query .= '(SELECT city FROM {addressbook_family} WHERE fid=addressbook_member.fid) city, ';
  $query .= '(SELECT country FROM {addressbook_family} WHERE fid=addressbook_member.fid) country, ';
  $query .= '(SELECT telephone FROM {addressbook_family} WHERE fid=addressbook_member.fid) telephone ';
  $query .= 'FROM {addressbook_member} ORDER BY last_name DESC, middle_name, first_name';

  $queryResult = db_query($query);

  fwrite($fp,'FIRST_NAME,MIDDLE_NAME,LAST_NAME,STREET,ZIPCODE,CITY,COUNTRY,TELEPHONE,MOBILE,EMAIL,BIRTH_DAY,WORK,NOTES,ACTIVE_ROLES,WANTED_ROLES');
  fwrite($fp,"\r\n");

  // Show all found members
  while ($data = db_fetch_object($queryResult))
  {
    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->first_name);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->middle_name);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->last_name);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->street);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->zipcode);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->city);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->country);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->telephone);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->mobile);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    // Replace found comma with space
    $tmp=str_replace(array(","), " ", $data->email);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    list($year, $month, $day) = split('[/.-]', $data->birth_day);
    fwrite($fp,$day.'-'.$month.'-'.$year);
    fwrite($fp,',');

    $tmp=str_replace(array("\r", "\n"), "~", $data->work);
    $tmp=str_replace(array(","), " ", $tmp);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    $tmp=str_replace(array("\r", "\n"), "~", $data->notes);
    $tmp=str_replace(array(","), " ", $tmp);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    $tmp=str_replace(array("\r", "\n"), "~", $data->active_roles);
    fwrite($fp,$tmp);
    fwrite($fp,',');

    $tmp=str_replace(array("\r", "\n"), "~", $data->wanted_roles);
    fwrite($fp,$tmp);
    fwrite($fp,"\r\n");
  }
  fclose($fp);

  header('HTTP/1.1 200 OK');
  header('Date: ' . date("D M j G:i:s T Y"));
  header('Last-Modified: ' . date("D M j G:i:s T Y"));
  header("Content-Type: application/force-download"); // changed to force download
  header("Content-Lenght: " . (string)(filesize($myFile)));
  header("Content-Transfer-Encoding: Binary"); // added
  header('Content-Disposition: attachment; filename="addressbook.csv"' );
  readfile($myFile);

  // Delete csv file
  unlink($myFile);
}

