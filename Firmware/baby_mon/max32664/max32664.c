



// This function handles all read commands or stated another way, all information
// requests. It starts a request by writing the family byte an index byte, and
// then delays 60 microseconds, during which the MAX32664 retrieves the requested 
// information. An I-squared-C request is then issued, and the information is read.
uint8_t readByte(uint8_t _familyByte, uint8_t _indexByte )
{
  uint8_t tmpBuf[2];
  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
 
  uint8_t returnByte;
  uint8_t statusByte;


   i2c_write(_i2c_device, tmpBuf, 2, _i2c_addr);

   k_msleep(CMD_DELAY);

   i2c_read(_i2c_device, tmpBuf, 2,  _i2c_addr);
   

   statusByte = tmpBuf[0];
   returnByte = tmpBuf[1];

  if( statusByte )// SUCCESS (0x00) - how do I know its 
    return statusByte; // Return the error, see: READ_STATUS_BYTE_VALUE 


  return returnByte; // If good then return the actual byte. 

}

// This function is exactly as the one above except it accepts also receives a 
// Write Byte as a paramter. It starts a request by writing the family byte, index byte, and
// write byte to the MAX32664 and then delays 60 microseconds, during which
// the MAX32664 retrieves the requested information. A I-squared-C request is
// then issued, and the information is read.
uint8_t  SparkFun_Bio_Sensor_Hub::readByte(uint8_t _familyByte, uint8_t _indexByte,\
                                           uint8_t _writeByte)
{
  uint8_t tmpBuf[3];
  uint8_t returnByte;
  uint8_t statusByte;

  tmpBuf[0] = _familyByte;
  tmpBuf[1] = _indexByte;
  tmpBuf[2] = _writeByte; 

  i2c_write(_i2c_device, tmpBuf, 3, _i2c_addr);

  k_msleep(CMD_DELAY);

  i2c_read(_i2c_device, tmpBuf, 2,  _i2c_addr);

  statusByte = tmpBuf[0];
  returnByte = tmpBuf[1];

  if( statusByte )// SUCCESS (0x00) - how do I know its 
    return statusByte; // Return the error, see: READ_STATUS_BYTE_VALUE 


  return returnByte; // If good then return the actual byte. 

}