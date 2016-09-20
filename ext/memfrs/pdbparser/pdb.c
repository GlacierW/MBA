/*
This file is part of MBA PDB Parser.
MBA PDB Parser Implementation.
This PDB parser is modified from RADARE2's repository.
Instead of using radare's API, we replace it with ut-serial library.

Copyright (c)   2016 ChongKuan Chen

This PDB Parser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utlist.h"

#include "types.h"
#include "pdb.h"
#include "limits.h"
#include "tpi.h"
#include "dbi.h"
#include "gdata.h"
#include "stream_pe.h"
#include "omap.h"

#define PDB7_SIGNATURE "Microsoft C/C++ MSF 7.00\r\n\x1A" "DS\0\0\0"
#define PDB7_SIGNATURE_LEN 32	

#include <stdbool.h>
#include "json.h"

/*******************************************************************
* NAME :            int addFieldToStruct( parent_obj, field_name, field_type, offset, size, is_pointer)
* DESCRIPTION :     Add a field into data structure information
* INPUTS :
*           json_object* parent_obj         json object of target data structure
*           char*        field_name         name of field
*           char*        field_type         type of firld
*           int          offset             offset of this field in the structure
*           bool         is_pointer         indicator to show if this field is a pointer     
*
* OUTPUTS :
*/
int addFieldToStruct(json_object *parent_obj, char* field_name, char* field_type, int offset, int size, bool is_pointer)
{
    json_object *field_info;
    field_info = json_object_new_array();

    json_object_array_add( field_info, json_object_new_string(field_type));
    json_object_array_add( field_info, json_object_new_int(offset));
    json_object_array_add( field_info, json_object_new_int(size));
    json_object_array_add( field_info, json_object_new_boolean(is_pointer));

    json_object_object_add( parent_obj, field_name, field_info);

}


int addGlobalVar(json_object *parent_obj, char* name, uint64_t offset, int size, char* section_name)
{
    json_object *var_info;
    var_info = json_object_new_array();

    //printf("%s\n\n", name);
    //json_object_array_add( field_info, json_object_new_string(field_type));
    json_object_array_add( var_info, json_object_new_int(offset));
    json_object_array_add( var_info, json_object_new_int(size));
    json_object_array_add( var_info, json_object_new_string(section_name));

    json_object_object_add( parent_obj, name, var_info);
}


typedef void (*parse_stream_)(void *stream, R_STREAM_FILE *stream_file);

typedef struct {
	int indx;
	parse_stream_ parse_stream;
	void *stream;
	EStream type;
	free_func free;
} SStreamParseFunc;

int count_pages(int length, int page_size) {
	int num_pages = 0;
	num_pages = length / page_size;
	if (length % page_size)
		num_pages++;
	return num_pages;
}

void build_name_field(char *name, char **name_field) {
	if (!name)
		*name_field = NULL;
	else
		*name_field = strdup (name);
}

int alloc_format_flag_and_member_fields(UT_array *ptmp,
				char **flags_format_field,
				int *members_amount,
				char ***members_name_field) {
	int i = 0;
	STypeInfo *it2 = NULL;
	int size = 0;

        while( (it2=(STypeInfo*)utarray_next(ptmp,it2))){
		*members_amount = *members_amount + 1;
	}

	if (!*members_amount) {
		return 0;
	}

	*flags_format_field = (char *) malloc(*members_amount + 1);
	memset(*flags_format_field, 0, *members_amount + 1);

	size = sizeof *members_name_field * (*members_amount);
	*members_name_field = (char **) malloc(size);
	for(i = 0; i < *members_amount; i++) {
		(*members_name_field)[i] = 0;
	}

	return 1;
}

void build_command_field(ELeafType lt, char **command_field) {
	switch (lt) {
	case eLF_STRUCTURE:
	case eLF_UNION:
		*command_field = (char *) malloc(strlen("pf") + 1);
		strcpy(*command_field, "pf");
		break;
	case eLF_ENUM:
		*command_field = (char *) malloc(strlen("\"td enum") + 1);
		strcpy(*command_field, "\"td enum");
		break;
	default:
		break;
	}
}

pdb_streams_element* utlist_get_n(pdb_streams_element* plist, int idx)
{
        int i = 1;
        pdb_streams_element* elt =NULL;
        DL_FOREACH(plist,elt){
            if(i==idx)
                return elt->pdb_stream;
            i++;
        }
}

//Currently, only structures are dump
void dump_json(R_PDB *pdb) {
	ELeafType lt = eLF_MAX;
	char *command_field = 0;
	char *name_field = 0;
	char *flags_format_field = 0; // format for struct
	char **members_name_field = 0;
	char *type = 0;
	int members_amount = 0;
	int i = 0;
	int pos = 0;
	char sym = ' ';
	int is_first = 1;
        char *type_name = NULL;
	char *name = NULL;
	int val = 0;
	int offset = 0;
	SType *t = 0;
        SType *it = NULL;
	STypeInfo *tf = NULL;
        STypeInfo *it2;
	pdb_streams_element *plist = pdb->pdb_streams;
        UT_array *ptmp = NULL;
  
        json_object *tyde_definition = NULL;
        json_object *jobj = NULL;

        STpiStream *tpi_stream = (STpiStream *)utlist_get_n(plist, ePDB_STREAM_TPI);

	if (!tpi_stream) {
		printf ("there is no tpi stream in current pdb\n");
		return;
	}

        // init json
        tyde_definition = json_object_new_object();

        while((it=(SType *)utarray_next(tpi_stream->types,it))){
		pos = 0;
		i = 0;
		members_amount = 0;
		val = 0;
                t = it;
		tf = &t->type_data;
		lt = tf->leaf_type;
		if ((tf->leaf_type == eLF_STRUCTURE) || (tf->leaf_type == eLF_UNION)
			|| (tf->leaf_type == eLF_ENUM)) {

                        // ENUM not yet implement
                        if( tf->leaf_type == eLF_ENUM ) continue;
                        
			if (tf->is_fwdref) {
				tf->is_fwdref(tf, &val);
				if (val == 1) {
					continue;
				}
			}

			is_first = 0;

                        jobj = json_object_new_object();

			if (tf->get_name)
				tf->get_name(tf, &type_name);
			// val for STRUCT or UNION mean size
			if (tf->get_val)
				tf->get_val(tf, &val);
			if (tf->get_members)
				tf->get_members(tf, (void*)&ptmp);
                         
                        
                        it2 = NULL;
                        tf = NULL;
                        offset = -1;
                        while(ptmp!=NULL && ( it2=(STypeInfo *)utarray_next(ptmp,it2))){
                                int index;
                                STypeInfo* tmp;
                                SType* stype;
         
                                tf = it2;
				if (tf->get_name)
					tf->get_name(tf, &name);
				if (tf->get_val)
					tf->get_val(tf, &offset);
				else
					offset = 0;
				if (tf->get_print_type)
					tf->get_print_type(tf, &type);
				
                                addFieldToStruct(jobj, name, type, offset, 0, false);
                                i++;
			}
                        //printf ("size- %x\n", val - offset);
                       
                        //filter
                        //printf( "Add to json %s\n", type_name);
                        json_object_object_add(tyde_definition, type_name, jobj);
                 }
        } // end of while
        json_object_to_file("type_definition.json", tyde_definition);
}

void dump_gvar_json(R_PDB *pdb)
{
	SStreamParseFunc *omap = 0, *sctns = 0, *sctns_orig = 0 , *gsym = 0, *tmp = 0;
	SIMAGE_SECTION_HEADER *sctn_header = 0;
	SGDATAStream *gsym_data_stream = 0;
	SPEStream *pe_stream = 0;
	SGlobal *gdata = 0;
        sstream_parse_func_element *it=NULL;
        
        json_object *global_var = json_object_new_object();

        DL_FOREACH(pdb->pdb_streams2 , it){
                tmp = it->sstream_parse_func;
		switch (tmp->type) {
		case ePDB_STREAM_SECT__HDR_ORIG:
			sctns_orig = tmp;
			break;
		case ePDB_STREAM_SECT_HDR:
			sctns = tmp;
			break;
		case ePDB_STREAM_OMAP_FROM_SRC:
			omap = tmp;
			break;
		case ePDB_STREAM_GSYM:
			gsym = tmp;
			break;
		default:
			break;
		}
	}

        if (!gsym) {
		printf ("there is no global symbols in current pdb\n");
		return;
	}

        // TODO: Add omap support 
  
        gsym_data_stream = (SGDATAStream *) gsym->stream;
	if ((omap != 0) && (sctns_orig != 0)) {
		pe_stream = (SPEStream *) sctns_orig->stream;
	} else {
		pe_stream = (SPEStream *) sctns->stream;
	}

        DL_FOREACH(gsym_data_stream->globals_list, gdata){
                sctn_header = utarray_eltptr(pe_stream->sections_hdrs, (gdata->segment -1));
                if (sctn_header) {
			char *name = gdata->name.name;
                        printf("name found: %s %x %d %s\n", name, sctn_header->virtual_address+gdata->offset, gdata->symtype, sctn_header->name);
                        addGlobalVar( global_var, name, sctn_header->virtual_address+gdata->offset, gdata->symtype, sctn_header->name);
                }
        }
        json_object_to_file("global_variable.json", global_var);
}

int init_pdb_parser(R_PDB *pdb, const char *filename) {
	char *signature = NULL;
	int bytes_read = 0;

	if (!pdb) {
		printf ("struct R_PDB is not correct\n");
		goto error;
	}
	

        pdb->fp = fopen (filename, "rb");

	if (!pdb->fp) {
		printf ("file %s can not be open\n", filename);
		goto error;
	}

	signature = (char *)calloc (1, PDB7_SIGNATURE_LEN);
	if (!signature) {
		printf ("memory allocation error\n");
		goto error;
	}

        bytes_read = fread(signature, PDB7_SIGNATURE_LEN, 1, pdb->fp);
        bytes_read *= PDB7_SIGNATURE_LEN;

	if (bytes_read != PDB7_SIGNATURE_LEN) {
		printf("file reading error\n");
		goto error;
	}

        fseek(pdb->fp, 0, SEEK_SET);

	if (!memcmp (signature, PDB7_SIGNATURE, PDB7_SIGNATURE_LEN)) {
	} else {
		goto error;
	}

	free(signature);

        pdb->pdb_streams = NULL;
	//pdb->print_types = print_types;
	printf("init_pdb_parser() finish with success\n");
	return 1;

error:
	free(signature);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
/// size - default value = -1
/// page_size - default value = 0x1000
///////////////////////////////////////////////////////////////////////////////
int init_r_pdb_stream(R_PDB_STREAM *pdb_stream, FILE *fp, int *pages,
		int pages_amount, int index, int size, int page_size) {
	pdb_stream->fp = fp;
	pdb_stream->pages = pages;
	pdb_stream->indx = index;
	pdb_stream->page_size = page_size;
	pdb_stream->pages_amount = pages_amount;

	if (size == -1) {
		pdb_stream->size =  pages_amount * page_size;
	} else {
		pdb_stream->size = size;
	}

	init_r_stream_file (&(pdb_stream->stream_file), fp, pages, pages_amount, size, page_size);
	//pdb_stream->free_ = free_pdb_stream;

	return 1;
}


int init_pdb7_root_stream(R_PDB *pdb, int *root_page_list, int pages_amount,
		EStream indx, int root_size, int page_size) {
	R_PDB_STREAM *pdb_stream = 0;
	int tmp_data_max_size = 0;
	char *tmp_data = NULL, *data_end;
	int stream_size = 0;
	int num_streams = 0;
	int *sizes = NULL;
	int num_pages = 0;
	int data_size = 0;
	char *data = NULL;
	int i = 0;
	int pos = 0;

	char *tmp;

	R_PDB7_ROOT_STREAM *root_stream7;

        pdb->root_stream = (R_PDB7_ROOT_STREAM*)calloc(1,sizeof(R_PDB7_ROOT_STREAM));
	init_r_pdb_stream(&pdb->root_stream->pdb_stream, pdb->fp, root_page_list, pages_amount,
					  indx, root_size, page_size);
 
        
	root_stream7 = pdb->root_stream;
	pdb_stream = &(root_stream7->pdb_stream);

	stream_file_get_size (&pdb_stream->stream_file, &data_size);
        
	data = (char *) calloc (1, data_size);
	stream_file_get_data(&pdb_stream->stream_file, data);
        
	num_streams = *(int *)data;
	tmp_data = data;
	tmp_data += 4;

	root_stream7->num_streams = num_streams;
	tmp_data_max_size = (data_size - (num_streams * 4 - 4));
	data_end = data + tmp_data_max_size;
        
	if (tmp_data_max_size> data_size) {
		free(data);
		printf ("invalid max tmp data size\n");
		return 0;
	}
	if (num_streams<0 || tmp_data_max_size <= 0) {
		free(data);
		printf ("too much amount of streams\n"
			   "curremt pdb file is not correct\n");
		return 0;
	}

	sizes = (int *) calloc (num_streams, 4);
	if (!sizes) {
		free(data);
		printf ("too much amount of streams\n"
			   "current pdb file is not correct\n");
		return 0;
	}

	for (i = 0; i < num_streams && (tmp_data+4 < data_end); i++) {
		stream_size = *(int *)(tmp_data);
		tmp_data += 4;
		if (stream_size == UINT_MAX)
			stream_size = 0;
		memcpy (sizes + i, &stream_size, 4);
	}
        
	tmp_data = ((char *)data + num_streams * 4 + 4);
        root_stream7->streams_list = NULL;
        
        SPage* pList = root_stream7->streams_list;
        
	SPage *page = 0;
	for (i = 0; i < num_streams; i++) {
		num_pages = count_pages (sizes[i], page_size);
		if ((pos + num_pages) > tmp_data_max_size) {
			free(data);
			free(sizes);
			printf("warning: looks like there is not correct values "
				   "of stream size in pdb file\n");
			return 0;
		}

		// TODO: cache that num_pages * 4, its used 4 times
		tmp = (char *) calloc (num_pages, 4);
		memset(tmp, 0, num_pages * 4);
                page = (SPage*) calloc(1, sizeof(SPage));
                printf("%d num_pages %d\n", i, num_pages); 
		if (num_pages != 0) {
			if ((pos+(num_pages*4)) > tmp_data_max_size) {
				printf ("data overrun by num_pages\n");
				free(data);
				free(sizes);
				free(tmp);
				free(page);
				return 0;
			}
			memcpy (tmp, tmp_data + pos, num_pages * 4);
			pos += num_pages * 4;
			page->stream_size = sizes[i];
			page->stream_pages = tmp;
			page->num_pages = num_pages;
		} else {
			page->stream_size = 0;
			page->stream_pages = 0;
			page->num_pages = 0;
			free(tmp);
		}
                
                DL_APPEND(root_stream7->streams_list, page);
                
	}
         
        //DL_FOREACH(root_stream7->streams_list, page) printf("%d ,", page->num_pages);
        //printf("\n");
	free (sizes);
	free (data);
        
	return 1;
}

void parse_pdb_info_stream(void *parsed_pdb_stream, R_STREAM_FILE *stream) {
	SPDBInfoStream *tmp = (SPDBInfoStream *)parsed_pdb_stream;

	tmp->names = 0;

	stream_file_read (stream, 4, (char *)&tmp->version);
	stream_file_read (stream, 4, (char *)&tmp->time_date_stamp);
	stream_file_read (stream, 4, (char *)&tmp->age);
	stream_file_read (stream, 4, (char *)&tmp->guid.data1);
	stream_file_read (stream, 2, (char *)&tmp->guid.data2);
	stream_file_read (stream, 2, (char *)&tmp->guid.data3);
	stream_file_read (stream, 8, (char *)&tmp->guid.data4);
	stream_file_read (stream, 4, (char *)&tmp->cb_names);

	tmp->names = (char *) calloc(1, tmp->cb_names);
	stream_file_read (stream, tmp->cb_names, tmp->names);
}

static void find_indx_in_list(sstream_parse_func_element *l, int index, SStreamParseFunc **res) {
        sstream_parse_func_element *stream_parse_func = NULL;

	*res = 0;
        DL_FOREACH( l, stream_parse_func){
		if (index == ((SStreamParseFunc*)(stream_parse_func->sstream_parse_func))->indx) {
			*res = stream_parse_func->sstream_parse_func;
			return;
		}
	}
}

#define ADD_INDX_TO_LIST(list, index, stream_size, stream_type, free_func, parse_func) { \
	if (index != -1) { \
                SStreamParseFunc *stream_parse_func = (SStreamParseFunc*)calloc(1, sizeof(SStreamParseFunc));\
		if (!stream_parse_func) return; \
		stream_parse_func->indx = (index); \
		stream_parse_func->type = (stream_type); \
		stream_parse_func->parse_stream = (parse_func); \
		stream_parse_func->free = (free_func); \
		if (stream_size) { \
			stream_parse_func->stream = malloc (stream_size); \
			if (!stream_parse_func->stream) { \
			        free(stream_parse_func); \
				return; \
			} \
		} else { \
			stream_parse_func->stream = 0; \
		} \
                sstream_parse_func_element* sstream_parse_ele = (sstream_parse_func_element*)calloc(1, sizeof(sstream_parse_func_element));\
                sstream_parse_ele->sstream_parse_func = stream_parse_func;\
                DL_APPEND(list, sstream_parse_ele);\
	} \
}

/*
ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = pdb_info_stream;
*/


//static void fill_list_for_stream_parsing(RList *l, SDbiStream *dbi_stream) {
static void fill_list_for_stream_parsing(sstream_parse_func_element **l, SDbiStream *dbi_stream) {
    ADD_INDX_TO_LIST(*l, dbi_stream->dbi_header.symrecStream, sizeof(SGDATAStream),
			ePDB_STREAM_GSYM, free_gdata_stream, parse_gdata_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_section_hdr, sizeof(SPEStream),
					 ePDB_STREAM_SECT_HDR, free_pe_stream, parse_pe_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_section_hdr_orig, sizeof(SPEStream),
					 ePDB_STREAM_SECT__HDR_ORIG, free_pe_stream, parse_pe_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_omap_to_src, sizeof(SOmapStream),
					 ePDB_STREAM_OMAP_TO_SRC, free_omap_stream, parse_omap_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_omap_from_src, sizeof(SOmapStream),
					 ePDB_STREAM_OMAP_FROM_SRC, free_omap_stream, parse_omap_stream);
}

int pdb_read_root(R_PDB *pdb) {
	int i = 0;

        pdb->pdb_streams = NULL;
	R_PDB7_ROOT_STREAM *root_stream = pdb->root_stream;
	R_PDB_STREAM *pdb_stream = 0;
	SPDBInfoStream *pdb_info_stream = 0;
	STpiStream *tpi_stream = 0;
	R_STREAM_FILE stream_file;
	SPage *page = 0;
	SStreamParseFunc *stream_parse_func = 0;
        pdb_streams_element *it;
        pdb_streams_element* ele;

        DL_FOREACH(root_stream->streams_list, page){
                stream_parse_func = 0;
		init_r_stream_file(&stream_file, pdb->fp, (int *)page->stream_pages,
						   page->num_pages/*root_stream->pdb_stream.pages_amount*/,
						   page->stream_size,
						   root_stream->pdb_stream.page_size);
                
		switch (i) {
		//TODO: rewrite for style like for streams from dbg stream
		//      look default
		case ePDB_STREAM_PDB:
                        pdb_info_stream = (SPDBInfoStream*)calloc(1, sizeof(SPDBInfoStream));
			
                        if (!pdb_info_stream) return 0;
			
                        //pdb_info_stream->free_ = free_info_stream;
			
                        parse_pdb_info_stream(pdb_info_stream, &stream_file);
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = pdb_info_stream;
                        DL_APPEND(pdb->pdb_streams, ele);
			break;
		case ePDB_STREAM_TPI:
                        tpi_stream = (STpiStream*)calloc(1, sizeof(STpiStream));

			if (!tpi_stream) return 0;
			init_tpi_stream(tpi_stream);
			if (!parse_tpi_stream(tpi_stream, &stream_file)) {
				free (tpi_stream);
				return 0;
			}
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = tpi_stream;
                        DL_APPEND(pdb->pdb_streams, ele);
                        printf("tpi stream %p\n", tpi_stream);
                        
			break;
		case ePDB_STREAM_DBI:
		{
                        SDbiStream *dbi_stream = (SDbiStream*)calloc(1,sizeof(SDbiStream));
			init_dbi_stream(dbi_stream);
                        
			parse_dbi_stream(dbi_stream, &stream_file);
			
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = dbi_stream;
                        DL_APPEND(pdb->pdb_streams, ele);

                        pdb->pdb_streams2 = NULL;
                        fill_list_for_stream_parsing(&(pdb->pdb_streams2), dbi_stream);
			break;
		}
		default:
			find_indx_in_list(pdb->pdb_streams2, i, &stream_parse_func);
			
                        if (stream_parse_func) {
				if (stream_parse_func->parse_stream) {
					stream_parse_func->parse_stream(stream_parse_func->stream,
													&stream_file);
					break;
				}
			}
 
                        /*
			pdb_stream = R_NEW0 (R_PDB_STREAM);
			init_r_pdb_stream(pdb_stream, pdb->buf, (int *)page->stream_pages,
							  root_stream->pdb_stream.pages_amount, i,
							  page->stream_size,
							  root_stream->pdb_stream.page_size);
			r_list_append(pList, pdb_stream);
                        */
			break;
		}
                

		if (stream_file.error) {
			return 0;
		}

		i++;
        }
    
	return 1;
}



int pdb7_parse(R_PDB *pdb) {
	char signature[PDB7_SIGNATURE_LEN + 1];
	int num_root_index_pages = 0;
	int *root_index_pages = 0;
	void *root_page_data = 0;
	int *root_page_list = 0;
	int num_root_pages = 0;
	int num_file_pages = 0;
	int alloc_tbl_ptr = 0;
	int bytes_read = 0;
	int page_size = 0;
	int root_size = 0;
	int reserved = 0;
	void *p_tmp;
	int i = 0;

        bytes_read = fread(signature, PDB7_SIGNATURE_LEN, 1, pdb->fp);
        bytes_read *= PDB7_SIGNATURE_LEN;

	if (bytes_read != PDB7_SIGNATURE_LEN) {
		printf ("error while reading PDB7_SIGNATURE\n");
		goto error;
	}

        fread(&page_size, 4, 1, pdb->fp);
        fread(&alloc_tbl_ptr, 4, 1, pdb->fp);
        fread(&num_file_pages, 4, 1, pdb->fp);
        fread(&root_size, 4, 1, pdb->fp);
        fread(&reserved, 4, 1, pdb->fp);

        printf("page_size %u\n", page_size);
        printf("alloc_tbl_ptr %u\n", alloc_tbl_ptr);
        printf("num_file_pages %u\n", num_file_pages);
        printf("root_size %u\n", root_size);
        printf("reserved %u\n", reserved);

	num_root_pages = count_pages(root_size, page_size);
	num_root_index_pages = count_pages((num_root_pages * 4), page_size);

	root_index_pages = (int *)calloc (sizeof(int), num_root_index_pages);
	if (!root_index_pages) {
		printf("error memory allocation\n");
		goto error;
	}

        bytes_read = fread(root_index_pages, 4, num_root_index_pages, pdb->fp);
        
	if (4*bytes_read != 4 * num_root_index_pages) {
		printf ("error while reading root_index_pages\n");
		goto error;
	}

	root_page_data = (int *)calloc(page_size, num_root_index_pages);
	if (!root_page_data) {
		printf ("error memory allocation of root_page_data\n");
		goto error;
	}

	p_tmp = root_page_data;
	for (i = 0; i < num_root_index_pages; i++) {
                fseek ( pdb->fp , root_index_pages[i] * page_size , SEEK_SET );
                fread(p_tmp, page_size, 1, pdb->fp );
		p_tmp = (char *)p_tmp + page_size;
	}

	root_page_list = (int *)calloc (sizeof(int), num_root_pages);
	if (!root_page_list) {
		printf ("error: memory allocation of root page\n");
		goto error;
	}

	p_tmp = root_page_data;
	for (i = 0; i < num_root_pages; i++) {
		root_page_list[i] = *((int *)p_tmp);
		p_tmp = (int *)p_tmp + 1;
	}

	if (!init_pdb7_root_stream (pdb, root_page_list, num_root_pages,
			ePDB_STREAM_ROOT, root_size, page_size)) {
		printf ("root stream has not initialized\n");
		goto error;
	}
        
	if (!pdb_read_root (pdb)) {
		printf ("pdb root has not initialized\n");
		goto error;
	}
        

	free(root_page_list);
	free(root_page_data);
	free(root_index_pages);
	return 1;

error:
	free(root_page_list);
	free(root_page_data);
	free(root_index_pages);
	return 0;
}



