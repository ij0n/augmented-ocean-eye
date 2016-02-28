#!/usr/bin/env python
"""Tests for ais.nmea_messages."""

import datetime
import unittest

from ais import nmea_messages


class AbkTest(unittest.TestCase):

  def testRegex(self):
    line = '$ANABK,,B,8,5,3*17'
    msg = nmea_messages.ABK_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'ack_type': '3',
         'chan': 'B',
         'checksum': '17',
         'mmsi': None,
         'msg_id': '8',
         'sentence': 'ABK',
         'seq_num': '5',
         'talker': 'AN'})

  def testDecode(self):
    line = '$ANABK,,A,8,4,3*15'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'ack_type': 3,
         'chan': 'A',
         'mmsi': None,
         'msg_id': 8,
         'msg': 'ABK',
         'seq_num': 4,
         'talker': 'AN'})


class AdsTest(unittest.TestCase):

  def testRegex(self):
    line = '$ANADS,L3 AIS ID,024358.79,V,0,I,I*3E'
    msg = nmea_messages.ADS_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'alarm': '',
         'checksum': '3E',
         'hours': '02',
         'id': 'L3 AIS ID',
         'minutes': '43',
         'pos_src': 'I',
         'seconds': '58.79',
         'sentence': 'ADS',
         'talker': 'AN',
         'time_src': 'I',
         'time_sync_method': '0',
         'time_utc': '024358.79'})

  def testDecode(self):
    line = '$ANADS,L3 AIS ID,024829.51,V,0,I,I*39'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'alarm': '',
         'id': 'L3 AIS ID',
         'msg': 'ADS',
         'pos_src': 'I',
         'talker': 'AN',
         'time_src': 'I',
         'time_sync_method': 0,
         'when': datetime.time(2, 48, 29, 510000)})

    line = '$BSADS,312670-BS,134839.00,A,3,N,N*22'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'alarm': '',
         'id': '312670-BS',
         'msg': 'ADS',
         'pos_src': 'N',
         'talker': 'BS',
         'time_src': 'N',
         'time_sync_method': 3,
         'when': datetime.time(13, 48, 39)})


class AlrTest(unittest.TestCase):

  def testRegex(self):
    line = '$ANALR,000000.00,007,A,V,AIS: UTC Lost*75'
    msg = nmea_messages.ALR_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'ack_state': 'V',
         'checksum': '75',
         'condition': 'A',
         'hours': '00',
         'id': '007',
         'minutes': '00',
         'seconds': '00.00',
         'sentence': 'ALR',
         'talker': 'AN',
         'text': 'AIS: UTC Lost',
         'time_utc': '000000.00'})

  def testDecode(self):
    line = '$ANALR,000000.00,007,A,V,AIS: UTC Lost*75'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'ack_state': False,
         'ack_state_raw': 'V',
         'condition': True,
         'condition_raw': 'A',
         'id': '007',
         'msg': 'ALR',
         'talker': 'AN',
         'text': 'AIS: UTC Lost',
         'time': datetime.time(0, 0)})

    line = '$BSALR,134239.00,002,A,V,AIS: Antenna VSWR exceeds limit*45'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'ack_state': False,
         'ack_state_raw': 'V',
         'condition': True,
         'condition_raw': 'A',
         'id': '002',
         'msg': 'ALR',
         'talker': 'BS',
         'text': 'AIS: Antenna VSWR exceeds limit',
         'time': datetime.time(13, 42, 39)})

    line = '$BSALR,000000.00,006,V,V,AIS: General Failure*0D,rbs1,1206655746.11'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'ack_state': False,
         'ack_state_raw': 'V',
         'condition': False,
         'condition_raw': 'V',
         'id': '006',
         'msg': 'ALR',
         'talker': 'BS',
         'text': 'AIS: General Failure',
         'time': datetime.time(0, 0)})

    line = '$BSALR,000000.00,051,V,V,AIS: IEC Comm Error*02,rbs1,1206655746.23'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'ack_state': False,
         'ack_state_raw': 'V',
         'condition': False,
         'condition_raw': 'V',
         'id': '051',
         'msg': 'ALR',
         'talker': 'BS',
         'text': 'AIS: IEC Comm Error',
         'time': datetime.time(0, 0)})


class BbmTest(unittest.TestCase):

  def testRegex(self):
    line = '!UPBBM,1,1,8,0,8,Fv4:Rb11Jq;=0Gjl:4vT80,4*06'
    msg = nmea_messages.BBM_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'body': 'Fv4:Rb11Jq;=0Gjl:4vT80',
         'chan': '0',
         'checksum': '06',
         'fill_bits': '4',
         'msg_id': '8',
         'sen_num': '1',
         'sen_tot': '1',
         'sentence': 'BBM',
         'seq_num': '8',
         'talker': 'UP'})

  def testDecode(self):
    line = '!UPBBM,1,1,2,0,8,Fv4:3s3QJr<R@GoB64vT80,4*22'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'body': 'Fv4:3s3QJr<R@GoB64vT80',
         'chan': 0,
         'fill_bits': 4,
         'msg': 'BBM',
         'msg_id': 8,
         'sen_num': 1,
         'sen_tot': 1,
         'seq_num': 2,
         'talker': 'UP'})


class FsrTest(unittest.TestCase):

  def testRegex(self):
    line = '$SAFSR,D07MN-CH-MTGBS1,000000,A,561,3,41,369,3,-122,696*0F'
    msg = nmea_messages.FSR_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'chan': 'A',
         'checksum': '0F',
         'crc_fails': '41',
         'hours': '00',
         'id': 'D07MN-CH-MTGBS1',
         'minutes': '00',
         'noise_db': '-122',
         'seconds': '00',
         'sentence': 'FSR',
         'slots_above_noise': '696',
         'slots_recv': '561',
         'slots_reserved': '369',
         'slots_reserved_self': '3',
         'slots_self': '3',
         'talker': 'SA',
         'time_utc': '000000'})

  def testDecodeFull(self):
    line = '$SAFSR,D09MN-SM-GULBS1,000000,B,115,3,5,86,3,-121,124*1F'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'chan': 'B',
         'crc_fails': 5,
         'id': 'D09MN-SM-GULBS1',
         'msg': 'FSR',
         'noise_db': -121,
         'slots_above_noise': 124,
         'slots_recv': 115,
         'slots_reserved': 86,
         'slots_reserved_self': 3,
         'slots_self': 3,
         'time': datetime.time(0, 0)})

  def testDecodeA(self):
    line = '$ARFSR,r17MANP1,000001,A,0005,0,0035,,,-128,*66'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'chan': 'A',
         'crc_fails': 35,
         'id': 'r17MANP1',
         'msg': 'FSR',
         'noise_db': -128,
         'slots_recv': 5,
         'slots_self': 0,
         'time': datetime.time(0, 0, 1)})

  def testDecodeX(self):
    line = '$ARFSR,b003669708,000004,X,488,0,,,,,*5B'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'chan': 'X',
         'id': 'b003669708',
         'msg': 'FSR',
         'slots_recv': 488,
         'slots_self': 0,
         'time': datetime.time(0, 0, 4)})


class GgaTest(unittest.TestCase):

  def testRegex(self):
    line = (
        '$GPGGA,174246.00,7119.6369,N,15640.8432,W,1,06,2.39,00011,M,000,M,,'
        '*7D')  # ',rakbarrow85,1226943363.79')
    msg = nmea_messages.GGA_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'antenna_height': '00011',
         'antenna_height_units': 'M',
         'checksum': '7D',
         'differential_age_sec': None,
         'differential_ref_station': '',
         'geoidal_height': '000',
         'geoidal_height_units': 'M',
         'gps_quality': '1',
         'hdop': '2.39',
         'hours': '17',
         'lat_deg': '71',
         'lat_min': '19.6369',
         'latitude': '7119.6369',
         'latitude_hemisphere': 'N',
         'lon_deg': '156',
         'lon_min': '40.8432',
         'longitude': '15640.8432',
         'longitude_hemisphere': 'W',
         'minutes': '42',
         'satellites': '06',
         'seconds': '46.00',
         'sentence': 'GGA',
         'talker': 'GP',
         'time_utc': '174246.00'})

  def testDecode(self):
    line = (
        '$GPGGA,174246.00,7119.6369,N,15640.8432,W,1,06,2.39,00011,M,000,M,,'
        '*7D')
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'antenna_height': 11,
         'antenna_height_units': 'M',
         'geoidal_height': 0,
         'geoidal_height_units': 'M',
         'gps_quality': 1,
         'hdop': 2.39,
         'latitude': 71.32728166666666,
         'longitude': -156.68072,
         'msg': 'GGA',
         'satellites': 6,
         'time': datetime.time(17, 42, 46)})


# TODO(schwehr): RMC.
# $GPRMC,150959.51,V,4234.8141,N,07039.8693,W,0.00,0.0,120308,15.1,W,N*21


class TxtTest(unittest.TestCase):

  def testRegex(self):
    # From an SR-162.
    line = '$AITXT,01,01,91,FREQ,2087,2088*57'
    msg = nmea_messages.TXT_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'checksum': '57',
         'sen_num': '01',
         'sen_tot': '01',
         'sentence': 'TXT',
         'seq_num': '91',
         'talker': 'AI',
         'text': 'FREQ,2087,2088'})

  def testDecode(self):
    line = '$AITXT,01,01,91,FREQ,2087,2088*57'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'msg': 'TXT',
         'sen_num': 1,
         'sen_tot': 1,
         'seq_num': 91,
         'talker': 'AI',
         'text': 'FREQ,2087,2088'})

    # TODO(schwehr): Add more tests from these.
    # $AITXT,01,01,01,WHALE NOTICES DATETIME 20110508T133349 UTC*58
    # $AITXT,01,01,70,Entered a DSC Receivable Window Chan A*46
    # $AITXT,01,01,70,Leaving a DSC Receivable Window^2C Chan A*7E
    # $AITXT,01,01,64,AIS: RATDMA Overflow*00,B0003160048,1064982808
    # $AITXT,1,1,007,AIS: UTC clock lost*08,1179172653.22


class ZdaTest(unittest.TestCase):

  def testRegex(self):
    line = '$INZDA,082015.0007,30,04,2009,,*73'
    msg = nmea_messages.ZDA_RE.search(line).groupdict()
    self.assertEqual(
        msg,
        {'checksum': '73',
         'day': '30',
         'hours': '08',
         'minutes': '20',
         'month': '04',
         'seconds': '15.0007',
         'sentence': 'ZDA',
         'talker': 'IN',
         'time_utc': '082015.0007',
         'year': '2009',
         'zone_hours': None,
         'zone_minutes': None})

  def testDecode(self):
    line = '$INZDA,082015.0007,30,04,2009,,*73'
    msg = nmea_messages.Decode(line)
    self.assertEqual(
        msg,
        {'msg': 'ZDA',
         'datetime': datetime.datetime(2009, 4, 30, 8, 20, 15, 700),
         'talker': 'IN',
         'zone_hours': None,
         'zone_minutes': None})


if __name__ == '__main__':
  unittest.main()
