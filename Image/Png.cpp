#include "stdafx.h"
#include "Png.h"

#include "../../Thirdparty/libpng/1.6.37/include/png.h"

typedef struct 
{ 
	const unsigned char *buffer; 
	png_uint_32 bufsize; 
	png_uint_32 current_pos; 

} MEMORY_READER_STATE; 
static void read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length) 
{ 
	MEMORY_READER_STATE *f = (MEMORY_READER_STATE *)png_get_io_ptr(png_ptr); 
	if (length > (f->bufsize - f->current_pos)) 
		png_error(png_ptr, "read error in read_data_memory (loadpng)"); 
	memcpy(data, f->buffer + f->current_pos, length); 
	f->current_pos += length;
}

CPng::CPng(const unsigned char* data,int len)
{
	CFile  file;
	if(file.Open(_T("c:\\1.png"),CFile::modeCreate | CFile::modeWrite)==TRUE)
	{
		file.Write(data,len);
		file.Close();
	}
	if (png_sig_cmp(data, 0, 8) == 0)
	{
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if(png_ptr == nullptr)
			return;
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(info_ptr == nullptr)
			return;

		MEMORY_READER_STATE memory_reader_state; 
		memory_reader_state.buffer = data; 
		memory_reader_state.bufsize = len; 
		memory_reader_state.current_pos = 0; 
		png_set_read_fn(png_ptr, &memory_reader_state, (png_rw_ptr)read_data_memory); 
		
		png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
		png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

		png_uint_32 channels   = png_get_channels(png_ptr, info_ptr);
		png_uint_32 bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

		switch (color_type) 
		{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png_ptr);
			//Don't forget to update the channel info (thanks Tom!)
			//It's used later to know how big a buffer we need for the image
			channels = 3;           
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (bit_depth < 8)
				png_set_expand_gray_1_2_4_to_8(png_ptr);
			//And the bitdepth info
			bit_depth = 8;
			break;
		}
		if (bit_depth == 16)
        	png_set_strip_16(png_ptr);
	}
}