//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// compress.c - Compress Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	gzipUncomp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pUnCompress			：解凍データを格納するポインタ
//		uncomprLen			：解凍データサイズを格納するポインタ
//		pUnCompress			：圧縮データを格納するポインタ
//		uncomprLen			：圧縮データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gzipUncomp (unsigned char *pUnCompress, int *uncomprLen, unsigned char *pCompress, int comprLen)
{
    int status;
    z_stream stream;
    int size;

    // Initialize
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in  = pCompress;
    stream.avail_in = 0;
    stream.next_out = pUnCompress;
    //stream.avail_in = 0;
    //stream.next_in = Z_NULL;
    if ((status = inflateInit2 (&stream, 47)) != Z_OK)
    {
    	sprintf (gLogMsgBuff, "Uncompress inflateInit2 Error. Error = %d\n", status);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

    // inflate
	size = comprLen;

	// output size
    stream.avail_out = (unsigned int)*uncomprLen;

    while (stream.total_out < *uncomprLen && stream.total_in < comprLen)
    {
	    // get compressed size
		if (size > UNCOMPRESS_BLOCK_SIZE)
			stream.avail_in = UNCOMPRESS_BLOCK_SIZE;
		else
			stream.avail_in = size;

        // Size Adjust
		size -= stream.avail_in;

        status = inflate (&stream, Z_NO_FLUSH);

        if (status == Z_NEED_DICT)
        	status = Z_DATA_ERROR;

        if (status == Z_MEM_ERROR || status == Z_DATA_ERROR)
        {
			sprintf (gLogMsgBuff, "Uncompress inflate Error. Error = %d\n", status);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
   			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
        }

        if (status == Z_STREAM_END)
            break;

		// Check Size
	    if (size == 0)
	    	break;
    }

    // End
    if ((status = inflateEnd (&stream)) != Z_OK)
    {
    	sprintf (gLogMsgBuff, "Uncompress inflateEnd Error. Error = %d\n", status);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

    // Uncompress Size
    *uncomprLen = stream.total_out;

_DONE:
	return (status);
}


//**********************************************************************************
//	gzipComp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pUnCompress			：解凍データを格納するポインタ
//		uncomprLen			：解凍データサイズ
//		pUnCompress			：圧縮データを格納するポインタ
//		uncomprLen			：圧縮データサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gzipComp (unsigned char *pUnCompress, int uncomprLen, unsigned char *pCompress, int *comprLen)
{
    int status;
    z_stream stream;
    int size;

    // Initialize
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in  = pUnCompress;
    stream.avail_in = 0;
    stream.next_out = pCompress;
    //stream.avail_in = 0;
    //stream.next_in = Z_NULL;
    if ((status = deflateInit2 (&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY)) != Z_OK)
    {
    	sprintf (gLogMsgBuff, "Compress deflateInit2 Error. Error = %d\n", status);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
    	goto _DONE;
    }

    // deflate
	size = uncomprLen;

	// output size
    stream.avail_out = (unsigned int)comprLen;

    while (stream.total_out < *comprLen && stream.total_in < uncomprLen)
    {
	    // get compressed size
		if (size > COMPRESS_BLOCK_SIZE)
			stream.avail_in = COMPRESS_BLOCK_SIZE;
		else
			stream.avail_in = size;

        // Size Adjust
		size -= stream.avail_in;

        status = deflate (&stream, Z_NO_FLUSH);

        if (status == Z_NEED_DICT)
        	status = Z_DATA_ERROR;

        if (status == Z_MEM_ERROR || status == Z_DATA_ERROR)
        {
        	sprintf (gLogMsgBuff, "Compress defflate Error. Error = %d\n", status);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
        }

        if (status == Z_STREAM_END)
            break;

		// Check Size
	    if (size == 0)
	    	break;
    }

    // Finish the stream, still forcing small buffers:
    while (stream.total_out < *comprLen)
    {
    	stream.avail_out = 1;
    	status = deflate(&stream, Z_FINISH);
        if (status == Z_STREAM_END)
        	break;
    }

    // End
    if ((status = deflateEnd (&stream)) != Z_OK)
    {
    	sprintf (gLogMsgBuff, "Compress deflateEnd Error. Error = %d\n", status);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_ACCESS_DENIED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

    // Compress Size
    *comprLen = stream.total_out;

_DONE:
	return (status);
}

// eof
