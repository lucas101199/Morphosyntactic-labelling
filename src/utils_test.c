/*
 * This file is subject to the terms and conditions defined in
 * file 'COPYING', which is part of this source code package.
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/common.h"
#include "utils/string.h"
#include "utils/array.h"
#include "utils/vector.h"
#include "utils/hashtable.h"
#include "utils/mapped.h"
#include "utils/debug.h"

#define fprintf(stream,format, ...) fprintf(stream, "%s:%d " format, __FILE__, __LINE__, ## __VA_ARGS__)

int int_comparator(const void*a, const void*b)
{
	int aa=*(int*)a;
	int bb=*(int*)b;
	if(aa<bb)return -1;
	if(aa>bb)return 1;
	return 0;
}

void print_callback(void* data, void* metadata)
{
	const char* format=(const char*)metadata;
	printf(format,*(int*)data);
}

void hashelement_print_callback(hashelement_t* element, void* metadata)
{
	fprintf(stdout,"%p ... %d\n",element->key,(int)element->value);
}

off_t saveValue(hashelement_t* element,void* metadata)
{
	return (off_t)element->value;
}

void* loadValue(const void* key,size_t key_length,off_t location,void* metadata)
{
	return (void*)location;
}

void test_debug2()
{
	debug(5,"print a message according to debug level (currently=%d)",debug_level);
	warn("this is a simple %s","warning");
	dump("there was no error, but I wanted to die...");
}

void test_debug1()
{
	test_debug2();
}

int main(int argc, char** argv)
{
#ifdef USE_GC
	GC_INIT(); // very important: the garbage collector needs to be initialized
	fprintf(stdout,"memory used: %lu\n",(unsigned long)GC_get_heap_size());
	char* dummy_pointer=MALLOC(1024*1024);
	fprintf(stdout,"memory used: %lu\n",(unsigned long)GC_get_heap_size());
#endif
//#ifdef DEBUG
	init_debugging(argv[0],0);
//#endif
	int i,j;
	fprintf(stdout,"---------- testing vectors\n");
	vector_t* vector=vector_new(3); // this is the initial memory size, not the actual length: the vector is still empty
	for(i=0;i<10;i++)
	{
		vector_push(vector,(void*)i);
		vector_set(vector,i,(void*)i);
		j=(int)vector_pop(vector);
		vector_push(vector,(void*)j);
		j=(int)vector_shift(vector);
		vector_unshift(vector,(void*)j);
	}
	fprintf(stdout,"vector length=%zd\n",vector->length);
	vector_t* vector2=vector_copy(vector);
	vector_append(vector,vector2);
	vector_prepend(vector,vector2);
	vector_fusion(vector,vector2); // will FREE vector2
	fprintf(stdout,"vector length=%zd\n",vector->length);
	vector_remove(vector,5,8);
	vector_remove_element(vector,23);
	vector_insert_element(vector,13,(void*)999);
	vector2=vector_new(555);
	vector_push(vector2,(void*)-555);
	vector_push(vector2,(void*)666);
	vector_push(vector2,(void*)777);
	vector_insert(vector,3,vector2);
	vector_free(vector2);
	vector_reverse(vector);
	vector_sort(vector,int_comparator);
	fprintf(stdout,"vector length=%zd\n",vector->length);
	for(i=0;i<vector->length;i++)
	{
		printf("%d, ",(int)vector_get(vector,i));
	}
	printf("\n");
	i=vector_search(vector,(void*)5);
	fprintf(stdout,"found [%d] at %d\n",(int)vector_get(vector,i),i);
	i=vector_search_sorted(vector,(void*)5,int_comparator);
	fprintf(stdout,"found [%d] at %d\n",(int)vector_get(vector,i),i);
	vector_remove_duplicates(vector);
	vector_apply(vector,print_callback,"%d ,");
	printf("\n");
	fprintf(stdout,"vector size=%zd bytes\n",vector_memory_size(vector));
	vector_optimize(vector);
	fprintf(stdout,"vector size=%zd bytes\n",vector_memory_size(vector));
	vector_free(vector);

	fprintf(stdout,"---------- testing arrays\n");
	array_t* array=array_new();
	for(i=0;i<10;i++)
	{
		array_push(array,(void*)i);
		array_set(array,i,(void*)i);
		j=(int)array_pop(array);
		array_push(array,(void*)j);
		j=(int)array_shift(array);
		array_unshift(array,(void*)j);
	}
	fprintf(stdout,"array length=%zd\n",array->length);
	array_t* array2=array_copy(array);
	array_append(array,array2);
	array_prepend(array,array2);
	array=array_fusion(array,array2); // will FREE array2
	fprintf(stdout,"array length=%zd\n",array->length);
	array_remove(array,5,8);
	array_remove_element(array,23);
	array_insert_element(array,13,(void*)999);
	array2=array_new(555);
	array_push(array2,(void*)-555);
	array_push(array2,(void*)666);
	array_push(array2,(void*)777);
	array_insert(array,3,array2);
	array_free(array2);
	array_reverse(array);
	array_sort(array,int_comparator);
	fprintf(stdout,"array length=%zd\n",array->length);
	for(i=0;i<array->length;i++)
	{
		printf("%d, ",(int)array_get(array,i));
	}
	printf("\n");
	i=array_search(array,(void*)5);
	fprintf(stdout,"found [%d] at %d\n",(int)array_get(array,i),i);
	array_remove_duplicates(array);
	array_apply(array,print_callback,"%d ,");
	printf("\n");
	fprintf(stdout,"array size=%zd bytes\n",array_memory_size(array));

	fprintf(stdout,"---------- testing conversions array <-> vector\n");
	vector=vector_from_array(array);
	array_free(array);
	vector_apply(vector,print_callback,"%d ,");
	printf("\n");
	array=array_from_vector(vector);
	vector_free(vector);
	array_apply(array,print_callback,"%d ,");
	printf("\n");
	array_free(array);

	fprintf(stdout,"---------- testing strings\n");
	string_t* string=string_new("l");
	string_prepend_cstr(string,"a");
	string_append_cstr(string,"a");
	string_t* string2=string_new("s");
	string_t* string3=string_new("d");
	string_prepend(string,string2);
	string_free(string2);
	string_append(string,string3);
	string_free(string3);
	fprintf(stdout,"%s\n",string->data);
	string2=string_new("ham");
	string3=string_new("cheese");
	array=array_new();
	array_push(array,string2);
	array_push(array,string3);
	array_push(array,string);
	string=string_new("bread");
	array_unshift(array,string);
	array_push(array,string);
	string=string_join_cstr(" + ",array);
	fprintf(stdout,"%s = a sandwich\n",string->data);
	for(i=0;i<array->length-1;i++) // warning, we added bread twice, so do not deallocate it twice
		string_free((string_t*)array_get(array,i));
	array_free(array);
	array=string_split(string,"\\+",NULL);
	array2=string_array_grep(array," .* ","!"); // reversed grep
	//array2=string_array_grep(array," .* ", ""); // reversed grep
	for(i=0;i<array2->length;i++)
	{
		fprintf(stdout,"grep found [%s]\n",((string_t*)array_get(array2,i))->data);
	}
	string_array_free(array);
	string_array_free(array2);
	string_chomp(string);
	string_reverse(string);
	fprintf(stdout,"%s = ???\n",string->data);
	vector_t* groups=NULL;
	while((groups=string_match(string,"E(.)","ci"))!=NULL)
	{
		fprintf(stdout,"[%s] [%s]\n", ((string_t*)vector_get(groups, 0))->data, ((string_t*)vector_get(groups, 1))->data);
		string_vector_free(groups);
	}
	if(string_match(string,"ee","n"))
		fprintf(stdout,"[%s] contains [ee]\n", string->data);
	else
		fprintf(stdout,"[%s] does not contain [ee]\n", string->data);
	if(string_match(string,"ww","n"))
		fprintf(stdout,"[%s] contains [ww]\n", string->data);
	else
		fprintf(stdout,"[%s] does not contain [ww]\n", string->data);
	string2=string_substr(string,4,10);
	fprintf(stdout,"%s\n",string2->data);
	string_free(string2);
	string_replace(string,"([ae]+)(.)","<\\$3$2-$1$99>","gi");
	fprintf(stdout,"%s\n",string->data);
	string_free(string);
	string3=string_new("3");
	int32_t string3_int=string_to_int32(string3);
	fprintf(stdout,"int=%d\n",string3_int);
	string_free(string3);
	string3=string_new("3.33333");
	float string3_float=string_to_float(string3);
	fprintf(stdout,"float=%f\n",string3_float);
	string_free(string3);
	string3=string_new("3.33333333333");
	double string3_double=string_to_double(string3);
	fprintf(stdout,"double=%.12f\n",string3_double);
	string_free(string3);
	string=string_sprintf("%d %f %f",string3_int, string3_float, string3_double);
	fprintf(stdout,"[%s]\n",string->data);
	string_free(string);

	fprintf(stdout,"---------- testing hashtables\n");
	hashtable_t* hashtable=hashtable_new();
	char* key1="foo";
	char* key2="bar";
	int key3=555;
	double key4=333.444;
	char* key_not_found="foo bar";
	hashtable_set(hashtable,key1,strlen(key1),(void*)666);
	hashtable_set(hashtable,key2,strlen(key2),(void*)777);
	hashtable_set(hashtable,&key3,sizeof(int),"example1");
	hashtable_set(hashtable,&key4,sizeof(double),"example2");
	hashtable_stats(hashtable,stdout);
	hashtable_optimize(hashtable);
	hashtable_stats(hashtable,stdout);
	char* value=hashtable_get(hashtable,&key3,sizeof(int));
	fprintf(stdout,"%d => %s\n",key3,value);
	value=hashtable_get(hashtable,key_not_found,strlen(key_not_found));
	fprintf(stdout,"%s => %s\n",key_not_found,value);
	i=(int)hashtable_get_or_default(hashtable,key_not_found,strlen(key_not_found),(void*)-1);
	fprintf(stdout,"%s => %d\n",key_not_found,i);
	hashtable_resize(hashtable,8);
	hashtable_stats(hashtable,stdout);
	i=(int)hashtable_get_or_default(hashtable,key1,strlen(key1),(void*)-1);
	fprintf(stdout,"%s => %d\n",key1,i);
	vector_t* elements=hashtable_elements(hashtable);
	for(i=0;i<elements->length;i++)
	{
		hashelement_t* element=(hashelement_t*)vector_get(elements,i);
		fprintf(stdout,"element => %p %d\n",element->key,(int)element->value);
	}
	vector_free(elements);
	vector_t* keys=hashtable_keys(hashtable);
	for(i=0;i<keys->length;i++)
	{
		fprintf(stdout,"key => %p\n",vector_get(keys,i));
	}
	vector_free(keys);
	vector_t* values=hashtable_values(hashtable);
	for(i=0;i<values->length;i++)
	{
		fprintf(stdout,"value => %d\n",(int)vector_get(values,i));
	}
	vector_free(values);
	for(value=(char*)hashtable_first_value(hashtable);value;value=(char*)hashtable_next_value(hashtable))
	{
		fprintf(stdout,"iter value=%d\n",(int)value);
	}
	char* removed=(char*)hashtable_remove(hashtable,&key3,sizeof(int));
	fprintf(stdout,"removed %d:%s\n",key3,removed);
	hashtable_apply(hashtable,hashelement_print_callback,NULL);
	FILE* file=fopen("foo.hashtable","w");
	if(file==NULL)die("foo.hashtable");
	//hashtable_save(hashtable,file,NULL,NULL); (equivalent to saving the void* in value)
	hashtable_save(hashtable,file,saveValue,NULL);
	hashtable_free(hashtable);
	fclose(file);
	file=fopen("foo.hashtable","r");
	if(file==NULL)die("foo.hashtable");
	//hashtable=hashtable_load(file,NULL,NULL); (equivalent to loading the value in void*)
	hashtable=hashtable_load(file,loadValue,NULL);
	hashtable_apply(hashtable,hashelement_print_callback,NULL); // you can observe that the keys get reallocated !!!
	hashtable_free(hashtable);
	fseek(file,0,SEEK_SET);
	off_t offset=hashtable_get_from_file(file,key1,strlen(key1));
	fprintf(stdout,"direct load: %s %d\n",key1,(int)offset);
	fclose(file);
	mapped_t* mapped=mapped_load_readonly("foo.hashtable");
	offset=hashtable_get_from_mapped(mapped,key1,strlen(key1));
	fprintf(stdout,"mapped load: %s %d\n",key1,(int)offset);
	mapped_free(mapped);
	unlink("foo.hashtable");

	fprintf(stdout,"---------- testing mapped\n");
	string_t* source_path=string_new(argv[0]);
	string_t* source_name=string_new(__FILE__);
	string_replace(source_path,"([^/]+)$",source_name->data,"o");
	mapped=mapped_load_readonly(source_path->data);
	string_free(source_name);
	string_free(source_path);
	char buffer[1024];
	for(i=0;i<10 && mapped_gets(mapped,buffer,1024);i++)
	{
		fprintf(stdout,"[%s]\n",buffer);
	}
	mapped_free(mapped);
	mapped=mapped_new("foo.mapped",strlen(key_not_found)+1);
	memcpy(mapped->data,key_not_found,strlen(key_not_found));
	*((char*)mapped->data+strlen(key_not_found))='\n';
	fprintf(stdout,"length=%d\n",(int)mapped->length);
	mapped_free(mapped);
	mapped=mapped_load("foo.mapped");
	mapped_resize(mapped,mapped->length+4);
	*((char*)mapped->data+mapped->length-1)='\n';
	*((char*)mapped->data+mapped->length-2)='y';
	*((char*)mapped->data+mapped->length-3)='e';
	*((char*)mapped->data+mapped->length-4)='H';
	fprintf(stdout,"length=%d\n",(int)mapped->length);
	mapped_free(mapped);
	unlink("foo.mapped");

#ifdef USE_GC
	*(dummy_pointer+1024*1024-1)='x';
	fprintf(stdout,"memory used: %lu\n",(unsigned long)GC_get_heap_size());
	GC_gcollect();
	fprintf(stdout,"memory used: %lu\n",(unsigned long)GC_get_heap_size());
#endif
	fprintf(stdout,"---------- testing debug\n");
	test_debug1();
	return 0;
}
