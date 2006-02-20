/*****************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * $Id$
 *
 * Example code that uploads a file name 'foo' to a remote script that accepts
 * "HTML form based" (as described in RFC1738) uploads using HTTP POST.
 *
 * The imaginary form we'll fill in looks like:
 *
 * <form method="post" enctype="multipart/form-data" action="examplepost.cgi">
 * Enter file: <input type="file" name="sendfile" size="40">
 * Enter file name: <input type="text" name="filename" size="30">
 * <input type="submit" value="send" name="submit">
 * </form>
 *
 * This exact source code has not been verified to work.
 */

#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

int main(int argc, char *argv[])
{
	CURL *curl;
	CURLcode res;

	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	char buf[] = "Expect:";
	char buffer[]="deine mutter \r\n foo & bar";

	curl_global_init(CURL_GLOBAL_ALL);

	/* email field */
	curl_formadd(&formpost,
				 &lastptr,
				 CURLFORM_COPYNAME, "email",
//				 CURLFORM_CONTENTTYPE, "form-data",
				 CURLFORM_COPYCONTENTS, "gumble@gmx.li",
				 CURLFORM_END);

	/* Fill in the file upload field */
	/* Fill in the filename field */
	curl_formadd(&formpost,
				 &lastptr,
				 CURLFORM_COPYNAME, "upfile",
				 CURLFORM_FILE, "/tmp/lssas.exe",
				 CURLFORM_END);
/*	curl_formadd(&formpost,
				 &lastptr,
				 CURLFORM_COPYNAME, "upfile",
				 CURLFORM_BUFFER, "buffertest",
				 CURLFORM_BUFFERPTR, buffer,
				 CURLFORM_BUFFERLENGTH, strlen(buffer),
				 CURLFORM_END);
*/
	curl = curl_easy_init();
	/* initalize custom header list (stating that Expect: 100-continue is not
	   wanted */
	headerlist = curl_slist_append(headerlist, buf);
	if ( curl )
	{
		/* what URL that receives this POST */
		curl_easy_setopt(curl, CURLOPT_URL, "http://sandbox.norman.no/live_4.html");
		if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )
			/* only disable 100-continue header if explicitly requested */
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		res = curl_easy_perform(curl);

		printf("Curl result is %s \n",curl_easy_strerror(res));
		/* always cleanup */
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */
		curl_formfree(formpost);
		/* free slist */
		curl_slist_free_all (headerlist);
	}
	return 0;
}
