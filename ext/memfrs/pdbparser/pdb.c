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

    printf("to_string()=%s\n", json_object_to_json_string(parent_obj));
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

	//it2 = r_list_iterator(ptmp);
	//while (r_list_iter_next(it2)) {
        while( (it2=(STypeInfo*)utarray_next(ptmp,it2))){
		//(void)r_list_iter_get(it2);
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
            //printf("%d ", i);
            if(i==idx)
                return elt->pdb_stream;
            i++;
        }
}

void print_types(R_PDB *pdb, int mode) {
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
	char *name = NULL;
	int val = 0;
	int offset = 0;
	SType *t = 0;
        SType *it = NULL;
	STypeInfo *tf = NULL;
        STypeInfo *it2;
	//RListIter *it = 0, *it2 = 0;
	pdb_streams_element *plist = pdb->pdb_streams;
        //*ptmp = NULL;
        UT_array *ptmp = NULL;
	//STpiStream *tpi_stream = r_list_get_n (plist, ePDB_STREAM_TPI);
        //STpiStream *tpi_stream = (STpiStream *)utlist_get_n(plist, ePDB_STREAM_TPI);
        //printf("print_types  ====================================\n");
        //printf("pdb %p\n", pdb);

        STpiStream *tpi_stream = (STpiStream *)utlist_get_n(plist, ePDB_STREAM_TPI);

	if (!tpi_stream) {
		printf ("there is no tpi stream in current pdb\n");
		return;
	}

	if (mode == 'j') {
		printf("{\"%s\":[", "types");
	}

	//it = r_list_iterator(tpi_stream->types);
	//while (r_list_iter_next(it)) {
        while((it=(SType *)utarray_next(tpi_stream->types,it))){
		pos = 0;
		i = 0;
		members_amount = 0;
		val = 0;
		//t = (SType *) r_list_iter_get(it);
                t = it;
		tf = &t->type_data;
		lt = tf->leaf_type;
		if ((tf->leaf_type == eLF_STRUCTURE) || (tf->leaf_type == eLF_UNION)
			|| (tf->leaf_type == eLF_ENUM)) {

			if (tf->is_fwdref) {
				tf->is_fwdref(tf, &val);
				if (val == 1) {
					continue;
				}
			}

			if ((mode == 8) && (is_first == 0)) {
				printf(",");
			}

			is_first = 0;

			if (tf->get_name)
				tf->get_name(tf, &name);
			// val for STRUCT or UNION mean size
			if (tf->get_val)
				tf->get_val(tf, &val);
			if (tf->get_members)
				tf->get_members(tf, (void*)&ptmp);
			printf ("%s: size 0x%x\n", name, val);
                         
			switch (mode) {
			case 'd': printf ("%s: size 0x%x\n", name, val); break;
			case 'r':
				build_command_field (lt, &command_field);
				build_name_field (name, &name_field);
				if (!alloc_format_flag_and_member_fields(ptmp,
							&flags_format_field,
							&members_amount,
							&members_name_field)) {
					goto err;
				}
				break;
			case 'j':
				switch (lt) {
				case eLF_ENUM:
					printf("{\"type\":\"%s\", \"name\":\"%s\",\"%s\":[",
								"enum", name , "enums");
					break;
				case eLF_STRUCTURE:
				case eLF_UNION:
					printf("{\"type\":\"%s\",\"name\":\"%s\",\"%s\":[",
								"structure", name, "members");
					break;
				default:
					continue;
				}

				break;
			}
                        
			//it2 = r_list_iterator(ptmp);
			//while (r_list_iter_next(it2)) {
                        it2 = NULL;
                        while(ptmp!=NULL && ( it2=(STypeInfo *)utarray_next(ptmp,it2))){
                                //printf("testtesttesttest\n");
				if ((mode == 'j') && (i)) {
					printf(",");
				}

				//tf = (STypeInfo *) r_list_iter_get(it2);
                                tf = it2;
				if (tf->get_name)
					tf->get_name(tf, &name);
				if (tf->get_val)
					tf->get_val(tf, &offset);
				else
					offset = 0;
				if (tf->get_print_type)
					tf->get_print_type(tf, &type);
				switch (mode) {
				case 'd':
					printf ("  0x%x: %s type:", offset, name);
					printf ("%s\n", type);
					break;
				case 'r':
					/*if (!build_flags_format_and_members_field(pdb, lt, name, type,
															i, &pos, offset,
															flags_format_field,
															members_name_field)) {
						R_FREE(type);
						goto err;
					}
					break;
                                        */
				case 'j': // JSON
					switch (lt) {
					case eLF_ENUM:
						printf("{\"%s\":\"%s\",\"%s\":%d}",
									"enum_name", name, "enum_val", offset);
						break;
					case eLF_STRUCTURE:
					case eLF_UNION:
						printf("{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":%d}",
									"member_type", type + strlen("(member)") + 1,
									"member_name", name, "offset", offset);
						break;
					default:
						break;
					}
					break;
				}

				free(type);
				i++;
			}
                        

			if (mode == 'r') {
				printf("%s %s ", command_field, name_field);
				if (lt != eLF_ENUM) {
					printf("%s ", flags_format_field);
				} else {
					printf("%c ", '{');
				}

				sym = (lt == eLF_ENUM) ? ',' : ' ';
				for (i = 0; i < members_amount; i++) {
					printf("%s", members_name_field[i]);
					if ((i + 1) != members_amount) {
						printf("%c", sym);
					}
				}

				if (lt == eLF_ENUM) {
					printf(" };\"\n");
				} else {
					printf("\n");
				}
			}

			if (mode == 'j') {
				printf("]}");
			}

err:
			if (mode == 'r') {
				free(command_field);
				free(name_field);
				free(flags_format_field);
				for (i = 0; i < members_amount; i++) {
					free(members_name_field[i]);
				}
				free(members_name_field);
			}
		}
	}

	if (mode == 'j') {
		printf("]}");
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
  
        //json 
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
			printf ("%s: size 0x%x\n", type_name, val);
                         
                        
                        it2 = NULL;
                        tf = NULL;
                        offset = -1;
                        while(ptmp!=NULL && ( it2=(STypeInfo *)utarray_next(ptmp,it2))){
                                int index;
                                //int prev_off;
                                STypeInfo* tmp;
                                SType* stype;
                                //calculate previous size
                                //if(tf == NULL)
                                //    prev_off = 0;
                                //if (tf->get_val)
                                //        tf->get_val(tf, &prev_off);
                                //else
                                //prev_off = offset;
         
                                tf = it2;
				if (tf->get_name)
					tf->get_name(tf, &name);
				if (tf->get_val)
					tf->get_val(tf, &offset);
				else
					offset = 0;
				if (tf->get_print_type)
					tf->get_print_type(tf, &type);
                                //if(prev_off != -1)
                                //        printf ("size-- %x\n", offset - prev_off);
				printf ("  0x%x: %s , type:", offset, name);
				printf ("%s ", type);
                                //GET TYPE 
                                //tmp = it2;                               
                                //if(tmp->get_index){
                                //    index = tmp->get_index(tmp, (void**)&stype);
                                //    if(stype){
                                //        printf("stype %x %x\n", stype->tpi_idx, stype->length);
                                //        printf("stype->info %x\n", stype->type_data.leaf_type);
                                //    } 
                                //}
				//free(type);

                                //int addFieldToStruct(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
				
                                addFieldToStruct(jobj, name, type, offset, 0, false);
                                i++;
			}
                        printf ("size- %x\n", val - offset);
                       
                        //filter
                        printf( "Add to json %s\n", type_name);
                        json_object_object_add(tyde_definition, type_name, jobj);
                        //json_object_to_file("type_definition.json", tyde_definition);
                        

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

        //l = pdb->pdb_streams2;
	//it = r_list_iterator (l);
	//while (r_list_iter_next (it)) {
        DL_FOREACH(pdb->pdb_streams2 , it){
		//tmp = (SStreamParseFunc *) r_list_iter_get(it);
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

        printf("sctns_orig %d, sctns %d, omap %d, gsym %d\n", sctns_orig, sctns, omap, gsym);
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

        //it = r_list_iterator(gsym_data_stream->globals_list);
	//while (r_list_iter_next(it)) {
        //        gdata = (SGlobal *) r_list_iter_get(it);
        DL_FOREACH(gsym_data_stream->globals_list, gdata){
                //sctn_header = r_list_get_n(pe_stream->sections_hdrs, (gdata->segment -1));
                sctn_header = utarray_eltptr(pe_stream->sections_hdrs, (gdata->segment -1));
                if (sctn_header) {
			char *name = gdata->name.name;
                        //printf("%x\n", sctn_header->virtual_address);
                        printf("name found: %s %x %d %s\n", name, sctn_header->virtual_address+gdata->offset, gdata->symtype, sctn_header->name);
                }
        }
}

int init_pdb_parser(R_PDB *pdb, const char *filename) {
	char *signature = NULL;
	int bytes_read = 0;

	if (!pdb) {
		printf ("struct R_PDB is not correct\n");
		goto error;
	}
	
        //if (!printf)
	//	printf = (PrintfCallback)printf;

//	pdb->buf = r_buf_new_slurp (filename);
        pdb->fp = fopen (filename, "rb");

//	pdb->fp = r_sandbox_fopen (filename, "rb");
	if (!pdb->fp) {
		printf ("file %s can not be open\n", filename);
		goto error;
	}

	signature = (char *)calloc (1, PDB7_SIGNATURE_LEN);
	if (!signature) {
		printf ("memory allocation error\n");
		goto error;
	}

	//bytes_read = r_buf_read(pdb->buf, (unsigned char *)signature, PDB7_SIGNATURE_LEN);
        bytes_read = fread(signature, PDB7_SIGNATURE_LEN, 1, pdb->fp);
        bytes_read *= PDB7_SIGNATURE_LEN;
        printf("%s size:%d read:%d\n", signature, PDB7_SIGNATURE_LEN, bytes_read);

	if (bytes_read != PDB7_SIGNATURE_LEN) {
		printf("file reading error\n");
		goto error;
	}

	//r_buf_seek(pdb->buf, 0, 0);
        fseek(pdb->fp, 0, SEEK_SET);

	if (!memcmp (signature, PDB7_SIGNATURE, PDB7_SIGNATURE_LEN)) {
		//pdb->pdb_parse = pdb7_parse;
	} else {
		goto error;
	}

	free(signature);

	//pdb->pdb_streams = r_list_new ();
        pdb->pdb_streams = NULL;
	//pdb->stream_map = 0;
	//pdb->finish_pdb_parse = finish_pdb_parse;
	pdb->print_types = print_types;
	//pdb->print_gvars = print_gvars;
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

	//pdb->root_stream = R_NEW0 (R_PDB7_ROOT_STREAM);
        pdb->root_stream = (R_PDB7_ROOT_STREAM*)calloc(1,sizeof(R_PDB7_ROOT_STREAM));
	init_r_pdb_stream(&pdb->root_stream->pdb_stream, pdb->fp, root_page_list, pages_amount,
					  indx, root_size, page_size);
 
        
	root_stream7 = pdb->root_stream;
	pdb_stream = &(root_stream7->pdb_stream);

	stream_file_get_size (&pdb_stream->stream_file, &data_size);
        printf("size in hex %x\n", data_size);
        
	data = (char *) calloc (1, data_size);
	stream_file_get_data(&pdb_stream->stream_file, data);
        
	num_streams = *(int *)data;
	tmp_data = data;
	tmp_data += 4;

	root_stream7->num_streams = num_streams;
        printf("num_stream %d\n", root_stream7->num_streams);
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
	//root_stream7->streams_list = r_list_new();
        root_stream7->streams_list = NULL;
        
	//RList *pList = root_stream7->streams_list;
        //element* pList = root_stream7->streams_list; 
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
		//page = R_NEW0 (SPage);
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
                
		//r_list_append (pList, page);
                DL_APPEND(root_stream7->streams_list, page);
                
	}
         
        DL_FOREACH(root_stream7->streams_list, page) printf("%d ,", page->num_pages);
        printf("\n");
	free (sizes);
	free (data);
        
	printf("init_pdb7_root_stream()\n");
	return 1;
}

void parse_pdb_info_stream(void *parsed_pdb_stream, R_STREAM_FILE *stream) {
	SPDBInfoStream *tmp = (SPDBInfoStream *)parsed_pdb_stream;

	tmp->names = 0;

	stream_file_read (stream, 4, (char *)&tmp->/*data.*/version);
	stream_file_read (stream, 4, (char *)&tmp->/*data.*/time_date_stamp);
	stream_file_read (stream, 4, (char *)&tmp->/*data.*/age);
	stream_file_read (stream, 4, (char *)&tmp->/*data.*/guid.data1);
	stream_file_read (stream, 2, (char *)&tmp->/*data.*/guid.data2);
	stream_file_read (stream, 2, (char *)&tmp->/*data.*/guid.data3);
	stream_file_read (stream, 8, (char *)&tmp->/*data.*/guid.data4);
	stream_file_read (stream, 4, (char *)&tmp->/*data.*/cb_names);

	tmp->/*data.*/names = (char *) calloc(1, tmp->/*data.*/cb_names);
	stream_file_read (stream, tmp->/*data.*/cb_names, tmp->/*data.*/names);
}

static void find_indx_in_list(sstream_parse_func_element *l, int index, SStreamParseFunc **res) {
	//SStreamParseFunc *stream_parse_func = NULL;
        sstream_parse_func_element *stream_parse_func = NULL;
	//RListIter *it = 0;

	*res = 0;
	//it = r_list_iterator(l);
	//while (r_list_iter_next(it)) {
        printf("start search %p\n", l);
        DL_FOREACH( l, stream_parse_func){
		//stream_parse_func = (SStreamParseFunc *) r_list_iter_get(it);
                printf("this stream %d\n", ((SStreamParseFunc*)(stream_parse_func->sstream_parse_func))->indx);
		if (index == ((SStreamParseFunc*)(stream_parse_func->sstream_parse_func))->indx) {
			*res = stream_parse_func->sstream_parse_func;
                        printf("%d index found\n", index);
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
                printf("add stream %d\n", stream_parse_func->indx);\
	} \
}

/*
ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = pdb_info_stream;
*/


//static void fill_list_for_stream_parsing(RList *l, SDbiStream *dbi_stream) {
static void fill_list_for_stream_parsing(sstream_parse_func_element **l, SDbiStream *dbi_stream) {
    printf("[fill_list_for_stream_parsing] %d\n", dbi_stream->dbi_header.symrecStream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbi_header.symrecStream, sizeof(SGDATAStream),
			ePDB_STREAM_GSYM, free_gdata_stream, parse_gdata_stream);
    printf("[fill_list_for_stream_parsing] %d %d\n", dbi_stream->dbg_header.sn_section_hdr, dbi_stream->dbg_header.sn_section_hdr_orig);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_section_hdr, sizeof(SPEStream),
					 ePDB_STREAM_SECT_HDR, free_pe_stream, parse_pe_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_section_hdr_orig, sizeof(SPEStream),
					 ePDB_STREAM_SECT__HDR_ORIG, free_pe_stream, parse_pe_stream);
    printf("[fill_list_for_stream_parsing] %d %d\n", dbi_stream->dbg_header.sn_omap_to_src, dbi_stream->dbg_header.sn_omap_from_src);

    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_omap_to_src, sizeof(SOmapStream),
					 ePDB_STREAM_OMAP_TO_SRC, free_omap_stream, parse_omap_stream);
    ADD_INDX_TO_LIST(*l, dbi_stream->dbg_header.sn_omap_from_src, sizeof(SOmapStream),
					 ePDB_STREAM_OMAP_FROM_SRC, free_omap_stream, parse_omap_stream);
}

int pdb_read_root(R_PDB *pdb) {
	int i = 0;

	//RList *pList = pdb->pdb_streams;
        pdb->pdb_streams = NULL;
	R_PDB7_ROOT_STREAM *root_stream = pdb->root_stream;
	R_PDB_STREAM *pdb_stream = 0;
	SPDBInfoStream *pdb_info_stream = 0;
	STpiStream *tpi_stream = 0;
	R_STREAM_FILE stream_file;
	//RListIter *it;
	SPage *page = 0;
	SStreamParseFunc *stream_parse_func = 0;
        pdb_streams_element *it;
        pdb_streams_element* ele;

        printf("pdb_read_root\n");
        printf("pdb %p\n", pdb);
        printf("%p\n", root_stream->streams_list);



        DL_FOREACH(root_stream->streams_list, page){
	//it = r_list_iterator(root_stream->streams_list);
	//while (r_list_iter_next(it)) {
	//	page = (SPage*) r_list_iter_get(it);
                stream_parse_func = 0;
		init_r_stream_file(&stream_file, pdb->fp, (int *)page->stream_pages,
						   page->num_pages/*root_stream->pdb_stream.pages_amount*/,
						   page->stream_size,
						   root_stream->pdb_stream.page_size);
                
		switch (i) {
		//TODO: rewrite for style like for streams from dbg stream
		//      look default
		case ePDB_STREAM_PDB:
			//pdb_info_stream = R_NEW0 (SPDBInfoStream);
                        pdb_info_stream = (SPDBInfoStream*)calloc(1, sizeof(SPDBInfoStream));
			
                        if (!pdb_info_stream) return 0;
			
                        //pdb_info_stream->free_ = free_info_stream;
			
                        parse_pdb_info_stream(pdb_info_stream, &stream_file);
			//r_list_append(pList, pdb_info_stream);
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = pdb_info_stream;
                        DL_APPEND(pdb->pdb_streams, ele);
			break;
		case ePDB_STREAM_TPI:
                         
			//tpi_stream = R_NEW0 (STpiStream);
                        tpi_stream = (STpiStream*)calloc(1, sizeof(STpiStream));

			if (!tpi_stream) return 0;
			init_tpi_stream(tpi_stream);
			if (!parse_tpi_stream(tpi_stream, &stream_file)) {
				free (tpi_stream);
				return 0;
			}
			//r_list_append(pList, tpi_stream);
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = tpi_stream;
                        DL_APPEND(pdb->pdb_streams, ele);
                        printf("tpi stream %p\n", tpi_stream);
                        
			break;
		case ePDB_STREAM_DBI:
		{
                        
			//SDbiStream *dbi_stream = R_NEW0 (SDbiStream);
                        SDbiStream *dbi_stream = (SDbiStream*)calloc(1,sizeof(SDbiStream));
			init_dbi_stream(dbi_stream);
                        
			parse_dbi_stream(dbi_stream, &stream_file);
			
                        //r_list_append(pList, dbi_stream);
                        ele = (pdb_streams_element*)calloc(1, sizeof(pdb_streams_element));
                        ele->pdb_stream = dbi_stream;
                        DL_APPEND(pdb->pdb_streams, ele);

                        /* 
			pdb->pdb_streams2 = r_list_new();
			fill_list_for_stream_parsing(pdb->pdb_streams2, dbi_stream);
                        */
                        pdb->pdb_streams2 = NULL;
                        fill_list_for_stream_parsing(&(pdb->pdb_streams2), dbi_stream);
			break;
		}
		default:
                        printf("Try to find %d stream\n", i);
			find_indx_in_list(pdb->pdb_streams2, i, &stream_parse_func);
			
                        if (stream_parse_func) {
                                printf("stream parse func found!!!!!\n");
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
                printf("%d\n", i);
        }
    
        printf("test pdb stream\n");
        DL_FOREACH(pdb->pdb_streams,it)
        {
            printf("%p\n", it );
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

	//bytes_read = r_buf_read(pdb->buf, (unsigned char *)signature, PDB7_SIGNATURE_LEN);
        printf("fp %p\n", &pdb);
        bytes_read = fread(signature, PDB7_SIGNATURE_LEN, 1, pdb->fp);
        bytes_read *= PDB7_SIGNATURE_LEN;

	if (bytes_read != PDB7_SIGNATURE_LEN) {
		printf ("error while reading PDB7_SIGNATURE\n");
		goto error;
	}

	/*if (!read_int_var ("page_size", &page_size, pdb))
		goto error;
	if (!read_int_var ("alloc_tbl_ptr", &alloc_tbl_ptr, pdb))
		goto error;
	if (!read_int_var ("num_file_pages", &num_file_pages, pdb))
		goto error;
	if (!read_int_var ("root_size", &root_size, pdb))
		goto error;
	if (!read_int_var("reserved", &reserved, pdb))
		goto error;
        */
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

//	bytes_read = fread(root_index_pages, 4, num_root_index_pages, pdb->fp);
	//bytes_read = r_buf_read(pdb->buf, (unsigned char *)root_index_pages, 4 * num_root_index_pages);
	//fread(root_index_pages, 4, num_root_index_pages, pdb->fp);
        bytes_read = fread(root_index_pages, 4, num_root_index_pages, pdb->fp);
        printf("%d\n", bytes_read);
        
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
		//r_buf_seek(pdb->buf, root_index_pages[i] * page_size, 0);
                fseek ( pdb->fp , root_index_pages[i] * page_size , SEEK_SET );
		//r_buf_read(pdb->buf, p_tmp, page_size);
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

	//pdb->pdb_streams2 = 0;
        
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



