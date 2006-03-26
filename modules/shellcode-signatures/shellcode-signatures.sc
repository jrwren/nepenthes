/* $Id$ */

/*
   =--- ShellcodeManager                                                      ---=
[x]   0) GenericXOR generic XOR decoder
[x]   1) GenericCreateProcess generic CreateProcess decoder
[x]   2) GenericUrl generic Url decoder
[x]   3) LinkXOR  link-bot XOR decoder
[x]   4) GenericCMD generic CMD decoder
[x]   5) LinkTrans handles linkbot/linkshellcode connectback transfers
[x]   6) LinkBindTrans handles linkbot/linkshellcode bind transfers
[x]   7) Stuttgart handles "stuttgart" shellcode
[ ]   8) Wuerzburg handles "wuerzburg" shellcode
[x]   9) GenericBind various bindshells
[x]   10) GenericConnect various bindshells
[x]   11) KonstanzXOR Konstanz XOR decoder
[x]   12) GenericConnectTrans various csends
[ ]   13) GenericUniCode generic UniCode decoder
[x]   14) GenericWinExec generic WinExec decoder
[x]   15) LeimbachUrlXOR generic XOR decoder
[x]   16) Genericwget generic wget decoder
[ ]   17) ASN1IISBase64 handles oc192 dcom bindshell
[ ]   18) ASN1SMBBind handles oc192 dcom bindshell
[ ]   19) THCConnect handles thc iis connectbackshells
[ ]   20) THCBind  handles thc iis bindshells
[ ]   21) HODBind  handles oc192 dcom bindshell
[ ]   22) HODConnect handles oc192 dcom bindshell
[ ]   23) HODBind  handles house of dabus msmq bindshells
[ ]   24) HODBind  handles house of dabus netdde bindshells
[ ]   25) HODConnect handles house of dabus netdde bindshells
[ ]   26) MandragoreBind mandragore sasserftpd bondshells
[ ]   27) MandragoreConnect mandragore sasserftpd bondshells
[ ]   28) HATSQUADConnect handles hat-squad wins connect
[ ]   29) HATSQUADBind handles hat-squad wins bindshell
[ ]   30) ZUCConnect handles zuc wins connect
   =--- 31 ShellcodeHandlers registerd  
*/

// taken from shellcode-generic/sch_generic_xor.cpp
xor::rbot64k
{
	pattern
	"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(.)"
	"\\xFF\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};


xor::rbot256c
{

	pattern
	"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\xB1(.)\\x80"
	"\\x73\\x0C(.)\\x43\\xE2\\xF9)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};

xor::bielefeld
{
	pattern
	"(.*)(\\xEB\\x10\\x5A\\x4A\\x33\\xC9\\x66\\xB9(..)\\x80\\x34\\x0A(.)\\xE2\\xFA"
	"\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};

xor::halle
{
	pattern
	"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x5B\\x31\\xC9\\x66\\xB9(..)"
	"\\x80\\x73\\x0E(.)\\x43\\xE2\\xF9)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};


xor::adenau
{
	pattern
	"(.*)(\\xEB\\x19\\x5E\\x31\\xC9\\x81\\xE9(....)\\x81\\x36(....)\\x81\\xEE\\xFC"
	"\\xFF\\xFF\\xFF\\xE2\\xF2\\xEB\\x05\\xE8\\xE2\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,sizeinvert,key,post);
};



xor::kaltenborn
{
	pattern
	"(.*)(\\xEB\\x03\\x5D\\xEB\\x05\\xE8\\xF8\\xFF\\xFF\\xFF\\x8B\\xC5\\x83\\xC0\\x11"
	"\\x33\\xC9\\x66\\xB9(..)\\x80\\x30(.)\\x40\\xE2\\xFA)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};

xor::deggendorf
{
	pattern
	"(.*)(\\xEB\\x10\\x5A\\x4A\\x31\\xC9\\x66\\xB9\(..)\\x80\\x34\\x0A(.)\\xE2\\xFA"
	"\\xEB\\x05\\xE8\\xEB\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post); 
};


xor::langenfeld
{
	pattern
	"(.*)(\\xEB\\x0F\\x5B\\x33\\xC9\\x66\\xB9(..)\\x80\\x33(.)\\x43\\xE2\\xFA\\xEB"
	"\\x05\\xE8\\xEC\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post);     
};

xor::saalfeld
{
	pattern
	"(.*)(\\xEB\\x03\\x5D\\xEB\\x05\\xE8\\xF8\\xFF\\xFF\\xFF\\x83\\xC5\\x15\\x90\\x90"
	"\\x90\\x8B\\xC5\\x33\\xC9\\x66\\xB9(..)\\x50\\x80\\x30(.)\\x40\\xE2\\xFA)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};  

xor::schoenberg
{
	pattern
	"(.*)(\\x31\\xC9\\x83\\xE9(.)\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13(....)"
	"\\x83\\xEB\\xFC\\xE2\\xF4)(.*)$";
	mapping (none,pre,pcre,key,sizeinvert,post);
};

xor::rosengarten
{
	pattern
	"(.*)(\\x33\\xC0\\xF7\\xD0\\x8B\\xFC\\xF2\\xAF\\x57\\x33\\xC9\\xB1(.)\\x90\\x90\\x90"
	"\\x90\\x80\\x37(.)\\x47\\xE2\\xFA.*\\xFF\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};

xor::schauenburg
{
	pattern
	"(.*)(\\xEB\\x0F\\x8B\\x34\\x24\\x33\\xC9\\x80\\xC1(.)\\x80\\x36(.)\\x46\\xE2\\xFA"
	"\\xC3\\xE8\\xEC\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post); 
};


xor::lichtenfels
{
	pattern
	"(.*)(\\xEB\\x02\\xEB\\x05\\xE8\\xF9\\xFF\\xFF\\xFF\\x58\\x83\\xC0\\x1B\\x8D\\xA0"
	"\\x01\\xFC\\xFF\\xFF\\x83\\xE4\\xFC\\x8B\\xEC\\x33\\xC9\\x66\\xB9(..)\\x80\\x30(.)"
	"\\x40\\xE2\\xFA)(.*)$";
	mapping (none,pre,pcre,size,key,post); 
};

xor::msfPexEnvSub
{
	pattern
	"(.*)(\\xC9\\x83\\xE9(.)\\xD9\\xEE\\xD9\\x74\\x24\\xF4\\x5B\\x81\\x73\\x13(....)\\x83"
	"\\xEB\\xFC\\xE2\\xF4)(.*)$";
	mapping (none,pre,pcre,key,sizeinvert,post);
};

xor::msfPex
{

	pattern
	"(.*)(\\x2B\\xC9\\x83\\xE9(.)\\xE8\\xFF\\xFF\\xFF\\xFF\\xC0\\x5E\\x81\\x76\\x0E(....)"
	"\\x83\\xEE\\xFC\\xE2\\xF4)(.*)$";
	mapping (none,pre,pcre,sizeinvert,key,post);
};


xor::leimbach
{
	pattern
	"(.*)(\\xEB\\x0E\\x5B\\x4B\\x33\\xC9\\xB1(.)\\x80\\x34\\x0B(.)\\xE2\\xFA\\xEB\\x05\\xE8"
	"\\xED\\xFF\\xFF\\xFF)(.*)$";
	mapping (none,pre,pcre,size,key,post); 
};

xor::mwcollect
{
	pattern
	"(.*)(\\xEB.\\xEB.\\xE8.*\\xB1(.).*\\x80..(.).*\\xE2.)(.*)$";
	mapping (none,pre,pcre,size,key,post);
};


// taken from shellcode-generic/sch_generic_linkxor.cpp

linkxor::link
{

/*
 * look at the source for information
 *
 */
	pattern
	"\\xEB\\x15\\xB9(....)\\x81\\xF1(....)\\x5E\\x80\\x74\\x31\\xFF(.)\\xE2\\xF9\\xEB\\x05\\xE8\\xE6\\xFF\\xFF\\xFF(.*)";
	mapping (key,key,size);
};

// taken from shellcode-generic/sch_generic_konstanz_xor.cpp
konstanzxor::konstanz
{
/*
 * xor key is index
 *
 */
	pattern
	"\\x33\\xC9\\x66\\xB9(..)\\xE8\\xFF\\xFF\\xFF\\xFF\\xC1\\x5E\\x30\\x4C\\x0E\\x07\\xE2\\xFA(.*)";
	mapping (key);
};


// taken from shellcode-generic/sch_generic_leimbach_url_xor.cpp

leimbachxor::leimbach
{   
	pattern
	"(.*)(\\xE9\\xBF\\x00\\x00\\x00\\x5F\\x64\\xA1\\x30\\x00\\x00\\x00\\x8B\\x40\\x0C\\x8B\\x70\\x1C"
	"\\xAD\\x8B\\x68\\x08\\x8B\\xF7\\x6A\\x03\\x59\\xE8\\x5F\\x00\\x00\\x00\\xE2\\xF9\\x68\\x6F\\x6E"
	"\\x00\\x00\\x68\\x75\\x72\\x6C\\x6D\\x54\\xFF\\x16\\x8B\\xE8\\xE8\\x49\\x00\\x00\\x00\\x8B\\xFE"
	"\\x83\\xC7\\x10\\x57\\x80\\x37(.)\\x47\\x80\\x3F(.)\\x75\\xF7\\x80\\x37\\x11\\x5F\\x83\\xEC\\x14"
	"\\x68\\x65\\x78\\x65\\x00\\x68\\x6F\\x73\\x74\\x2E\\x68\\x73\\x76\\x63\\x68\\x68\\x65\\x72\\x73"
	"\\x5C\\x68\\x64\\x72\\x69\\x76\\x8B\\xDC\\x33\\xC0\\x50\\x50\\x53\\x57\\x50\\xFF\\x56\\x0C\\x85"
	"\\xC0\\x75\\x07\\x8B\\xDC\\x50\\x53\\xFF\\x56\\x04\\xFF\\x56\\x08\\x51\\x56\\x8B\\x45\\x3C\\x8B"
	"\\x54\\x28\\x78\\x03\\xD5\\x52\\x8B\\x72\\x20\\x03\\xF5\\x33\\xC9\\x49\\x41\\xAD\\x03\\xC5\\x33"
	"\\xDB\\x0F\\xBE\\x10\\x3A\\xD6\\x74\\x08\\xC1\\xCB\\x0D\\x03\\xDA\\x40\\xEB\\xF1\\x3B\\x1F\\x75"
	"\\xE7\\x5A\\x8B\\x5A\\x24\\x03\\xDD\\x66\\x8B\\x0C\\x4B\\x8B\\x5A\\x1C\\x03\\xDD\\x8B\\x04\\x8B"
	"\\x03\\xC5\\xAB\\x5E\\x59\\xC3\\xE8\\x3C\\xFF\\xFF\\xFF................)(.*)$";

	mapping (key,key);
};


// taken from shellcode-generic/shellcode-generic.conf.dist


bindshell::mainz
{
	pattern
	"\\x50\\x50\\x50\\x50\\x6A\\x01\\x6A\\x02\\xFF\\x57\\xEC\\x8B\\xD8\\xC7\\x07\\x02\\x00(..)\\x33\\xC0"
	"\\x89\\x47\\x04\\x6A\\x10\\x57\\x53\\xFF\\x57\\xF0\\x6A\\x01\\x53\\xFF\\x57\\xF4\\x50\\x50\\x53\\xFF"
	"\\x57\\xF8";

	mapping (port);
};

bindshell::adenau
{
	pattern 

"\\x83\\xEC\\x34\\x8B\\xF4\\xE8\\x47\\x01\\x00\\x00\\x89\\x06\\xFF\\x36\\x68\\x8E\\x4E\\x0E"
"\\xEC\\xE8\\x61\\x01\\x00\\x00\\x89\\x46\\x08\\xFF\\x36\\x68\\xAD\\xD9\\x05\\xCE\\xE8\\x52"
"\\x01\\x00\\x00\\x89\\x46\\x0C\\x68\\x6C\\x6C\\x00\\x00\\x68\\x33\\x32\\x2E\\x64\\x68\\x77"
"\\x73\\x32\\x5F\\x54\\xFF\\x56\\x08\\x89\\x46\\x04\\xFF\\x36\\x68\\x72\\xFE\\xB3\\x16\\xE8"
"\\x2D\\x01\\x00\\x00\\x89\\x46\\x10\\xFF\\x36\\x68\\x7E\\xD8\\xE2\\x73\\xE8\\x1E\\x01\\x00"
"\\x00\\x89\\x46\\x14\\xFF\\x76\\x04\\x68\\xCB\\xED\\xFC\\x3B\\xE8\\x0E\\x01\\x00\\x00\\x89"
"\\x46\\x18\\xFF\\x76\\x04\\x68\\xD9\\x09\\xF5\\xAD\\xE8\\xFE\\x00\\x00\\x00\\x89\\x46\\x1C"
"\\xFF\\x76\\x04\\x68\\xA4\\x1A\\x70\\xC7\\xE8\\xEE\\x00\\x00\\x00\\x89\\x46\\x20\\xFF\\x76"
"\\x04\\x68\\xA4\\xAD\\x2E\\xE9\\xE8\\xDE\\x00\\x00\\x00\\x89\\x46\\x24\\xFF\\x76\\x04\\x68"
"\\xE5\\x49\\x86\\x49\\xE8\\xCE\\x00\\x00\\x00\\x89\\x46\\x28\\xFF\\x76\\x04\\x68\\xE7\\x79"
"\\xC6\\x79\\xE8\\xBE\\x00\\x00\\x00\\x89\\x46\\x2C\\x33\\xFF\\x81\\xEC\\x90\\x01\\x00\\x00"
"\\x54\\x68\\x01\\x01\\x00\\x00\\xFF\\x56\\x18\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF"
"\\x56\\x1C\\x8B\\xD8\\x57\\x57\\x68\\x02\\x00(..)\\x8B\\xCC\\x6A\\x16\\x51\\x53\\xFF\\x56"
"\\x20\\x57\\x53\\xFF\\x56\\x24\\x57\\x51\\x53\\xFF\\x56\\x28\\x8B\\xD0\\x68\\x65\\x78\\x65"
"\\x00\\x68\\x63\\x6D\\x64\\x2E\\x89\\x66\\x30\\x83\\xEC\\x54\\x8D\\x3C\\x24\\x33\\xC0";


/*
	"\\x83\\xEC\\x34\\x8B\\xF4\\xE8\\x47\\x01\\x00\\x00\\x89\\x06\\xFF\\x36\\x68\\x8E\\x4E\\x0E\\xEC\\xE8"
	"\\x61\\x01\\x00\\x00\\x89\\x46\\x08\\xFF\\x36\\x68\\xAD\\xD9\\x05\\xCE\\xE8\\x52\\x01\\x00\\x00\\x89"
	"\\x46\\x0C\\x68\\x6C\\x6C\\x00\\x00\\x68\\x33\\x32\\x2E\\x64\\x68\\x77\\x73\\x32\\x5F\\x54\\xFF\\x56"
	"\\x08\\x89\\x46\\x04\\xFF\\x36\\x68\\x72\\xFE\\xB3\\x16\\xE8\\x2D\\x01\\x00\\x00\\x89\\x46\\x10\\xFF"
	"\\x36\\x68\\x7E\\xD8\\xE2\\x73\\xE8\\x1E\\x01\\x00\\x00\\x89\\x46\\x14\\xFF\\x76\\x04\\x68\\xCB\\xED"
	"\\xFC\\x3B\\xE8\\x0E\\x01\\x00\\x00\\x89\\x46\\x18\\xFF\\x76\\x04\\x68\\xD9\\x09\\xF5\\xAD\\xE8\\xFE"
	"\\x00\\x00\\x00\\x89\\x46\\x1C\\xFF\\x76\\x04\\x68\\xA4\\x1A\\x70\\xC7\\xE8\\xEE\\x00\\x00\\x00\\x89"
	"\\x46\\x20\\xFF\\x76\\x04\\x68\\xA4\\xAD\\x2E\\xE9\\xE8\\xDE\\x00\\x00\\x00\\x89\\x46\\x24\\xFF\\x76"
	"\\x04\\x68\\xE5\\x49\\x86\\x49\\xE8\\xCE\\x00\\x00\\x00\\x89\\x46\\x28\\xFF\\x76\\x04\\x68\\xE7\\x79"
	"\\xC6\\x79\\xE8\\xBE\\x00\\x00\\x00\\x89\\x46\\x2C\\x33\\xFF\\x81\\xEC\\x90\\x01\\x00\\x00\\x54\\x68"
	"\\x01\\x01\\x00\\x00\\xFF\\x56\\x18\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF\\x56\\x1C\\x8B\\xD8"
	"\\x57\\x57\\x68\\x02\\x00(..)\\x8B\\xCC\\x6A\\x16\\x51\\x53\\xFF\\x56\\x20\\x57\\x53\\xFF\\x56\\x24"
	"\\x57\\x51\\x53\\xFF\\x56\\x28\\x8B\\xD0\\x68\\x65\\x78\\x65\\x00\\x68\\x63\\x6D\\x64\\x2E\\x89\\x66"
	"\\x30\\x83\\xEC\\x54\\x8D\\x3C\\x24\\x33\\xC0";
*/
	mapping (port);
};

bindshell::kaltenborn
{
	pattern     
	"\\xFF\\x56\\xF4\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF\\x56\\xF0\\x8B\\xD8\\x57\\x57\\x68\\x02"
	"\\x00(..)\\x8B\\xCC\\x6A\\x16\\x51\\x53\\xFF\\x56\\xEC\\x57\\x53\\xFF\\x56\\xE8\\x33\\xFF\\x57\\x51"
	"\\x53\\xFF\\x56\\xE2\\x8B\\xD0\\x89\\x46\\xBE\\x68\\x63\\x6D\\x64\\x00\\x89\\x66\\xC2\\x83\\xC4\\xAC"
	"\\x8D\\x3C\\x24\\x33\\xC0\\x33\\xC9\\x80\\xC1\\x15\\xAB\\xE2\\xFD\\xC6\\x44\\x24\\x10\\x44\\xFE\\x44"
	"\\x24\\x3D\\x89\\x54\\x24\\x48\\x89\\x54\\x24\\x4C\\x89\\x54\\x24\\x50\\x8D\\x44\\x24\\x10\\x54\\x50"
	"\\x51\\x51\\x51\\x41\\x51\\x49\\x51\\x51\\xFF\\x76\\xC2\\x51\\xFF\\x56\\xCE\\x8B\\xCC\\x6A\\xFF\\xFF"
	"\\x31\\xFF\\x56\\xD2\\x8B\\xC8\\xFF\\x76\\xBE\\xFF\\x56\\xD6\\xEB\\x9E\\xFF\\x56\\x14";

	mapping (port);
};

bindshell::wackerow
{
	pattern     
	"\\xE8\\x7C\\x00\\x00\\x00\\x83\\xC6\\x0D\\x52\\x56\\xFF\\x57\\xFC\\x5A\\x8B\\xD8\\x6A\\x04\\x59\\xE8"
	"\\x69\\x00\\x00\\x00\\x50\\x50\\x50\\x50\\x6A\\x01\\x6A\\x02\\xFF\\x57\\xF0\\x8B\\xD8\\xC7\\x07\\x02"
	"\\x00(..)\\x33\\xC0\\x89\\x47\\x04\\x6A\\x10\\x57\\x53\\xFF\\x57\\xF4\\x6A\\x01\\x53\\xFF\\x57\\xF8"
	"\\x50\\x50\\x53\\xFF\\x57\\xFC\\x83\\xEC\\x44\\x8B\\xF4\\x33\\xDB\\x6A\\x10\\x59\\x89\\x1C\\x8E\\xE2"
	"\\xFB\\x89\\x46\\x38\\x89\\x46\\x3C\\x89\\x46\\x40\\xC7\\x46\\x2C\\x01\\x01\\x00\\x00\\x8D\\x47\\x10"
	"\\x50\\x56\\x53\\x53\\x53\\x6A\\x01\\x53\\x53\\xC7\\x47\\x3C\\x63\\x6D\\x64\\x00\\x8D\\x47\\x3C\\x50"
	"\\x53\\xFF\\x57\\xE4\\x50\\xFF\\x57\\xE8";

	mapping (port);
};

bindshell::parthenstein
{
	pattern             
	"\\xFF\\x56\\x18\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF\\x56\\x1C\\x8B\\xD8\\x57\\x57\\x68\\x02"
	"\\x00(..)\\x8B\\xCC\\x6A\\x16\\x51\\x53\\xFF\\x56\\x20\\x57\\x53\\xFF\\x56\\x24\\x57\\x51\\x53\\xFF"
	"\\x56\\x28\\x8B\\xD0\\x68\\x65\\x78\\x65\\x00\\x68\\x63\\x6D\\x64\\x2E\\x89\\x66\\x30\\x83\\xEC\\x54"
	"\\x8D\\x3C\\x24\\x33\\xC0\\x33\\xC9\\x83\\xC1\\x15\\xAB\\xE2\\xFD\\xC6\\x44\\x24\\x10\\x44\\xFE\\x44"
	"\\x24\\x3D\\x89\\x54\\x24\\x48\\x89\\x54\\x24\\x4C\\x89\\x54\\x24\\x50\\x8D\\x44\\x24\\x10\\x54\\x50"
	"\\x51\\x51\\x51\\x6A\\x01\\x51\\x51\\xFF\\x76\\x30\\x51\\xFF\\x56\\x10\\x8B\\xCC\\x6A\\xFF\\xFF\\x31"
	"\\xFF\\x56\\x0C\\x8B\\xC8\\x57\\xFF\\x56\\x2C\\xFF\\x56\\x14";

	mapping (port);
};

bindshell::schoenborn
{
	pattern             
	"\\xFC\\x6A\\xEB\\x4D\\xE8\\xF9\\xFF\\xFF\\xFF\\x60\\x8B\\x6C\\x24\\x24\\x8B\\x45\\x3C\\x8B\\x7C\\x05"
	"\\x78\\x01\\xEF\\x8B\\x4F\\x18\\x8B\\x5F\\x20\\x01\\xEB\\x49\\x8B\\x34\\x8B\\x01\\xEE\\x31\\xC0\\x99"
	"\\xAC\\x84\\xC0\\x74\\x07\\xC1\\xCA\\x0D\\x01\\xC2\\xEB\\xF4\\x3B\\x54\\x24\\x28\\x75\\xE5\\x8B\\x5F"
	"\\x24\\x01\\xEB\\x66\\x8B\\x0C\\x4B\\x8B\\x5F\\x1C\\x01\\xEB\\x03\\x2C\\x8B\\x89\\x6C\\x24\\x1C\\x61"
	"\\xC3\\x31\\xDB\\x64\\x8B\\x43\\x30\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x40\\x08\\x5E\\x68\\x8E"
	"\\x4E\\x0E\\xEC\\x50\\xFF\\xD6\\x66\\x53\\x66\\x68\\x33\\x32\\x68\\x77\\x73\\x32\\x5F\\x54\\xFF\\xD0"
	"\\x68\\xCB\\xED\\xFC\\x3B\\x50\\xFF\\xD6\\x5F\\x89\\xE5\\x66\\x81\\xED\\x08\\x02\\x55\\x6A\\x02\\xFF"
	"\\xD0\\x68\\xD9\\x09\\xF5\\xAD\\x57\\xFF\\xD6\\x53\\x53\\x53\\x53\\x53\\x43\\x53\\x43\\x53\\xFF\\xD0"
	"\\x66\\x68(..)\\x66\\x53\\x89\\xE1\\x95\\x68\\xA4\\x1A\\x70\\xC7\\x57\\xFF\\xD6\\x6A\\x10\\x51\\x55"
	"\\xFF\\xD0\\x68\\xA4\\xAD\\x2E\\xE9\\x57\\xFF\\xD6\\x53\\x55\\xFF\\xD0\\x68\\xE5\\x49\\x86\\x49\\x57"
	"\\xFF\\xD6\\x50\\x54\\x54\\x55\\xFF\\xD0\\x93\\x68\\xE7\\x79\\xC6\\x79\\x57\\xFF\\xD6\\x55\\xFF\\xD0"
	"\\x66\\x6A\\x64\\x66\\x68\\x63\\x6D\\x89\\xE5\\x6A\\x50\\x59\\x29\\xCC\\x89\\xE7\\x6A\\x44\\x89\\xE2"
	"\\x31\\xC0\\xF3\\xAA\\xFE\\x42\\x2D\\xFE\\x42\\x2C\\x93\\x8D\\x7A\\x38\\xAB\\xAB\\xAB\\x68\\x72\\xFE"
	"\\xB3\\x16\\xFF\\x75\\x44\\xFF\\xD6\\x5B\\x57\\x52\\x51\\x51\\x51\\x6A\\x01\\x51\\x51\\x55\\x51\\xFF"
	"\\xD0\\x68\\xAD\\xD9\\x05\\xCE\\x53\\xFF\\xD6\\x6A\\xFF\\xFF\\x37\\xFF\\xD0\\x8B\\x57\\xFC\\x83\\xC4"
	"\\x64\\xFF\\xD6\\x52\\xFF\\xD0\\x68\\xEF\\xCE\\xE0\\x60\\x53\\xFF\\xD6\\xFF\\xD0";

	mapping (port); 
};

bindshell::ravensburg
{
	pattern             
	"\\xEB\\x23(..)\\x02\\x05\\x6C\\x59\\xF8\\x1D\\x9C\\xDE\\x8C\\xD1\\x4C\\x70\\xD4\\x03\\xF0\\x27\\x20"
	"\\x20\\x30\\x08\\x57\\x53\\x32\\x5F\\x33\\x32\\x2E\\x44\\x4C\\x4C\\x01\\xEB\\x05\\xE8\\xF9\\xFF\\xFF"
	"\\xFF\\x5D\\x83\\xED\\x2A\\x6A\\x30\\x59\\x64\\x8B\\x01\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x78"
	"\\x08\\x8D\\x5F\\x3C\\x8B\\x1B\\x01\\xFB\\x8B\\x5B\\x78\\x01\\xFB\\x8B\\x4B\\x1C\\x01\\xF9\\x8B\\x53"
	"\\x24\\x01\\xFA\\x53\\x51\\x52\\x8B\\x5B\\x20\\x01\\xFB\\x31\\xC9\\x41\\x31\\xC0\\x99\\x8B\\x34\\x8B"
	"\\x01\\xFE\\xAC\\x31\\xC2\\xD1\\xE2\\x84\\xC0\\x75\\xF7\\x0F\\xB6\\x45\\x05\\x8D\\x44\\x45\\x04\\x66"
	"\\x39\\x10\\x75\\xE1\\x66\\x31\\x10\\x5A\\x58\\x5E\\x56\\x50\\x52\\x2B\\x4E\\x10\\x41\\x0F\\xB7\\x0C"
	"\\x4A\\x8B\\x04\\x88\\x01\\xF8\\x0F\\xB6\\x4D\\x05\\x89\\x44\\x8D\\xD8\\xFE\\x4D\\x05\\x75\\xBE\\xFE"
	"\\x4D\\x04\\x74\\x21\\xFE\\x4D\\x22\\x8D\\x5D\\x18\\x53\\xFF\\xD0\\x89\\xC7\\x6A\\x04\\x58\\x88\\x45"
	"\\x05\\x80\\x45\\x77\\x0A\\x8D\\x5D\\x74\\x80\\x6B\\x26\\x14\\xE9\\x78\\xFF\\xFF\\xFF\\x89\\xCE\\x31"
	"\\xDB\\x53\\x53\\x53\\x53\\x56\\x46\\x56\\xFF\\xD0\\x97\\x55\\x58\\x66\\x89\\x30\\x6A\\x10\\x55\\x57"
	"\\xFF\\x55\\xD4\\x4E\\x56\\x57\\xFF\\x55\\xCC\\x53\\x55\\x57\\xFF\\x55\\xD0\\x97\\x8D\\x45\\x88\\x50"
	"\\xFF\\x55\\xE4\\x55\\x55\\xFF\\x55\\xE8\\x8D\\x44\\x05\\x0C\\x94\\x53\\x68\\x2E\\x65\\x78\\x65\\x68"
	"\\x5C\\x63\\x6D\\x64\\x94\\x31\\xD2\\x8D\\x45\\xCC\\x94\\x57\\x57\\x57\\x53\\x53\\xFE\\xC6\\x01\\xF2"
	"\\x52\\x94\\x8D\\x45\\x78\\x50\\x8D\\x45\\x88\\x50\\xB1\\x08\\x53\\x53\\x6A\\x10\\xFE\\xCE\\x52\\x53"
	"\\x53\\x53\\x55\\xFF\\x55\\xEC\\x6A\\xFF\\xFF\\x55\\xE0";

	mapping (port);
};

bindshell::schauenburg
{
	pattern             
	"\\xBE\\xCC\\x10\\xBE\\x77\\x68\\x33\\x32\\x00\\x00\\x68\\x77\\x73\\x32\\x5F\\x54\\xFF\\x15\\xD0\\x10"
	"\\xBE\\x77\\x97\\x99\\x52\\x52\\x52\\x52\\x42\\x52\\x42\\x52\\xE8\\x0B\\x00\\x00\\x00\\x57\\x53\\x41"
	"\\x53\\x6F\\x63\\x6B\\x65\\x74\\x41\\x00\\x57\\xFF\\x16\\xFF\\xD0\\x93\\x6A\\x00\\x68\\x02\\x00(..)"
	"\\x8B\\xC4\\x6A\\x10\\x50\\x53\\xE8\\x05\\x00\\x00\\x00\\x62\\x69\\x6E\\x64\\x00\\x57\\xFF\\x16\\xFF"
	"\\xD0\\x6A\\x01\\x53\\xE8\\x07\\x00\\x00\\x00\\x6C\\x69\\x73\\x74\\x65\\x6E\\x00\\x57\\xFF\\x16\\xFF"
	"\\xD0\\x6A\\x00\\x54\\x53\\xE8\\x07\\x00\\x00\\x00\\x61\\x63\\x63\\x65\\x70\\x74\\x00\\x57\\xFF\\x16"
	"\\xFF\\xD0\\x8B\\xD8\\x33\\xC0\\x6A\\x10\\x59\\x8B\\xFC\\x57\\x03\\xF9\\x57\\xF3\\xAB\\x8B\\x3C\\x24"
	"\\x50\\x50\\x50\\x6A\\x01\\x50\\x50\\xE8\\x04\\x00\\x00\\x00\\x63\\x6D\\x64\\x00\\x50\\xE8\\x0F\\x00"
	"\\x00\\x00\\x43\\x72\\x65\\x61\\x74\\x65\\x50\\x72\\x6F\\x63\\x65\\x73\\x73\\x41\\x00\\xC6\\x07\\x44"
	"\\xC7\\x47\\x2C\\x01\\x01\\x00\\x00\\x83\\xC7\\x38\\x93\\xAB\\xAB\\xAB\\x64\\x67\\xA1\\x30\\x00\\x8B"
	"\\x40\\x0C\\x8B\\x40\\x1C\\x8B\\x00\\xFF\\x70\\x08\\xFF\\x16\\xFF\\xD0\\xEB";
	mapping (port);
};






connectbackshell::bielefeld
{
	pattern
	"\\xc7\\x02\\x63\\x6d\\x64\\x00\\x52\\x50\\xff\\x57\\xe8\\xc7\\x07\\x02\\x00(..)\\xc7\\x47\\x04"
	"(....)\\x6a\\x10\\x57\\x53\\xff\\x57\\xf8\\x53\\xff\\x57\\xfc\\x50\\xff\\x57\\xec";
	mapping (port,host);
};


connectbackshell::konstanz
{
	pattern
	"\\xff\\xd0\\x68(....)\\x66\\x68(..)\\x66\\x53\\x89\\xe1\\x95\\x68\\xec\\xf9\\xaa\\x60\\x57\\xff\\xd6"
	"\\x6a\\x10\\x51\\x55\\xff\\xd0";
	mapping (host,port);    
};  


connectbackshell::egghunter
{
	pattern     
	"\\x41\\x42\\x41\\x42\\x41\\x42\\x41\\x42\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\x90\\xFC\\x6A\\xEB"
	"\\x52\\xE8\\xF9\\xFF\\xFF\\xFF\\x60\\x8B\\x6C\\x24\\x24\\x8B\\x45\\x3C\\x8B\\x7C\\x05\\x78\\x01\\xEF"
	"\\x83\\xC7\\x01\\x8B\\x4F\\x17\\x8B\\x5F\\x1F\\x01\\xEB\\xE3\\x30\\x49\\x8B\\x34\\x8B\\x01\\xEE\\x31"
	"\\xC0\\x99\\xAC\\x84\\xC0\\x74\\x07\\xC1\\xCA\\x0D\\x01\\xC2\\xEB\\xF4\\x3B\\x54\\x24\\x28\\x75\\xE3"
	"\\x8B\\x5F\\x23\\x01\\xEB\\x66\\x8B\\x0C\\x4B\\x8B\\x5F\\x1B\\x01\\xEB\\x03\\x2C\\x8B\\x89\\x6C\\x24"
	"\\x1C\\x61\\xC3\\x31\\xC0\\x64\\x8B\\x40\\x30\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x40\\x08\\x5E"
	"\\x68\\x8E\\x4E\\x0E\\xEC\\x50\\xFF\\xD6\\x31\\xDB\\x66\\x53\\x66\\x68\\x33\\x32\\x68\\x77\\x73\\x32"
	"\\x5F\\x54\\xFF\\xD0\\x68\\xCB\\xED\\xFC\\x3B\\x50\\xFF\\xD6\\x5F\\x89\\xE5\\x66\\x81\\xED\\x08\\x02"
	"\\x55\\x6A\\x02\\xFF\\xD0\\x68\\xD9\\x09\\xF5\\xAD\\x57\\xFF\\xD6\\x53\\x53\\x53\\x53\\x43\\x53\\x43"
	"\\x53\\xFF\\xD0\\x68(....)\\x66\\x68(..)\\x66\\x53\\x89\\xE1\\x95\\x68\\xEC\\xF9\\xAA\\x60\\x57\\xFF"
	"\\xD6\\x6A\\x10\\x51\\x55\\xFF\\xD0\\x66\\x6A\\x64\\x66\\x68\\x63\\x6D\\x6A\\x50\\x59\\x29\\xCC\\x89"
	"\\xE7\\x6A\\x44\\x89\\xE2\\x31\\xC0\\xF3\\xAA\\x95\\x89\\xFD\\xFE\\x42\\x2D\\xFE\\x42\\x2C\\x8D\\x7A"
	"\\x38\\xAB\\xAB\\xAB\\x68\\x72\\xFE\\xB3\\x16\\xFF\\x75\\x28\\xFF\\xD6\\x5B\\x57\\x52\\x51\\x51\\x51"
	"\\x6A\\x01\\x51\\x51\\x55\\x51\\xFF\\xD0\\x68\\xAD\\xD9\\x05\\xCE\\x53\\xFF\\xD6\\x6A\\xFF\\xFF\\x37"
	"\\xFF\\xD0\\x68\\xE7\\x79\\xC6\\x79\\xFF\\x75\\x04\\xFF\\xD6\\xFF\\x77\\xFC\\xFF\\xD0\\x68\\xEF\\xCE"
	"\\xE0\\x60\\x53\\xFF\\xD6\\xFF\\xD0";
	mapping (host,port);    
};  

connectbackshell::langenfeld
{
	pattern
	"\\xE9\\xF4\\x00\\x00\\x00\\x5A\\xB8\\x0C\\xF0\\xFD\\x7F\\x8B\\x00\\x8B\\x70\\x1C\\xAD\\x8B\\x40\\x08"
	"\\x8B\\xD8\\x8B\\x73\\x3C\\x03\\xF3\\x8B\\x76\\x78\\x03\\xF3\\x8B\\x7E\\x20\\x03\\xFB\\x8B\\x4E\\x14"
	"\\x33\\xED\\x56\\x57\\x51\\x8B\\x3F\\x03\\xFB\\x8B\\xF2\\x6A\\x0E\\x59\\xF3\\xA6\\x74\\x08\\x59\\x5F"
	"\\x83\\xC7\\x04\\x45\\xE2\\xE9\\x59\\x5F\\x5E\\x8B\\xCD\\x8B\\x46\\x24\\x03\\xC3\\xD1\\xE1\\x03\\xC1"
	"\\x33\\xC9\\x66\\x8B\\x08\\x8B\\x46\\x1C\\x03\\xC3\\xC1\\xE1\\x02\\x03\\xC1\\x8B\\x00\\x03\\xC3\\x8B"
	"\\xFA\\x8B\\xF7\\x83\\xC6\\x0E\\x8B\\xD0\\x6A\\x03\\x59\\xE8\\x70\\x00\\x00\\x00\\x83\\xC6\\x0D\\x52"
	"\\x56\\xFF\\x57\\xFC\\x5A\\x8B\\xD8\\x6A\\x02\\x59\\xE8\\x5D\\x00\\x00\\x00\\x83\\xEC\\x44\\x8B\\xF4"
	"\\x6A\\x10\\x59\\x89\\x04\\x8E\\xE2\\xFB\\x50\\x50\\x50\\x50\\x6A\\x01\\x6A\\x02\\xFF\\x57\\xF8\\x8B"
	"\\xD8\\x89\\x5E\\x38\\x89\\x5E\\x3C\\x89\\x5E\\x40\\x66\\xC7\\x46\\x2C\\x01\\x01\\x8D\\x47\\x10\\x50"
	"\\x56\\x33\\xC0\\x50\\x50\\x50\\x6A\\x01\\x50\\x50\\x8D\\x57\\x3C\\xC7\\x02\\x63\\x6D\\x64\\x00\\x52"
	"\\x50\\xFF\\x57\\xEC\\xC7\\x07\\x02\\x00(..)\\xC7\\x47\\x04(....)\\x6A\\x10\\x57\\x53\\xFF\\x57\\xFC"
	"\\x50\\xFF\\x57\\xF0";
	mapping (port,host);    
};  

connectbackshell::pinneberg
{
	pattern
	"\\xE8\\x04\\x01\\x00\\x00\\x89\\x46\\x04\\xFF\\x36\\x68\\x72\\xFE\\xB3\\x16\\xE8\\xF5\\x00\\x00\\x00"
	"\\x89\\x46\\x08\\xFF\\x36\\x68\\xEF\\xCE\\xE0\\x60\\xE8\\xE6\\x00\\x00\\x00\\x89\\x46\\x0C\\x68\\x33"
	"\\x32\\x00\\x00\\x68\\x77\\x73\\x32\\x5F\\x54\\xFF\\x56\\x04\\x89\\x46\\x10\\xFF\\x76\\x10\\x68\\xD9"
	"\\x09\\xF5\\xAD\\xE8\\xC5\\x00\\x00\\x00\\x89\\x46\\x14\\xFF\\x76\\x10\\x68\\xEC\\xF9\\xAA\\x60\\xE8"
	"\\xB5\\x00\\x00\\x00\\x89\\x46\\x18\\xFF\\x76\\x10\\x68\\xE7\\x79\\xC6\\x79\\xE8\\xA5\\x00\\x00\\x00"
	"\\x89\\x46\\x1C\\xFF\\x76\\x10\\x68\\xCB\\xED\\xFC\\x3B\\xE8\\x95\\x00\\x00\\x00\\x89\\x46\\x20\\x81"
	"\\xEC\\x90\\x01\\x00\\x00\\x54\\x68\\x01\\x01\\x00\\x00\\xFF\\x56\\x20\\x50\\x50\\x50\\x50\\x40\\x50"
	"\\x40\\x50\\xFF\\x56\\x14\\x8B\\xD8\\x68(....)\\x68\\x02\\x00(..)\\x8B\\xCC\\x6A\\x10\\x51\\x53\\xFF"
	"\\x56\\x18\\x85\\xC0\\x75\\x43\\x68\\x63\\x6D\\x64\\x00\\x89\\x66\\x30\\x83\\xEC\\x54\\x8D\\x3C\\x24"
	"\\x33\\xC9\\x83\\xC1\\x15\\xAB\\xE2\\xFD\\xC6\\x44\\x24\\x10\\x44\\xFE\\x44\\x24\\x3D\\x89\\x5C\\x24"
	"\\x48\\x89\\x5C\\x24\\x4C\\x89\\x5C\\x24\\x50\\x8D\\x44\\x24\\x10\\x54\\x50\\x51\\x51\\x51\\x6A\\x01"
	"\\x51\\x51\\xFF\\x76\\x30\\x51\\xFF\\x56\\x08\\x53\\xFF\\x56\\x1C\\xFF\\x56\\x0C";
	mapping (host,port);    
};  


connectbackshell::lichtenfels
{
	pattern     
	"\\xFF\\x57\\xF0\\x5A\\x8B\\xD8\\x33\\xC9\\xB1\\x04\\xE8\\x87\\x00\\x00\\x00\\x83\\xC6\\x08\\x55\\x68"
	"\\x01\\x01\\x00\\x00\\xFF\\x57\\xF0\\x85\\xC0\\x75\\x73\\x50\\x50\\x50\\x50\\x40\\x50\\x40\\x50\\xFF"
	"\\x57\\xF4\\x83\\xF8\\xFF\\x74\\x63\\x8B\\xD8\\x66\\xC7\\x45\\x00\\x02\\x00\\x66\\xC7\\x45\\x02(..)"
	"\\xC7\\x45\\x04(....)\\x6A\\x10\\x55\\x53\\xFF\\x57\\xFC\\x85\\xC0\\x75\\x43\\x33\\xC9\\xB1\\x11\\x57"
	"\\x8B\\xFD\\xF3\\xAB\\x5F\\xC7\\x45\\x00\\x44\\x00\\x00\\x00\\x89\\x5D\\x3C\\x89\\x5D\\x38\\x89\\x5D"
	"\\x40\\xC7\\x45\\x2C\\x01\\x01\\x00\\x00\\x8D\\x45\\x44";
	mapping (host,port);    
};  

connectbackshell::msf_win32_reverse
{
	pattern
	"\\xfc\\x6a\\xeb\\x4d\\xe8\\xf9\\xff\\xff\\xff\\x60\\x8b\\x6c\\x24\\x24\\x8b\\x45\\x3c\\x8b\\x7c\\x05"
	"\\x78\\x01\\xef\\x8b\\x4f\\x18\\x8b\\x5f\\x20\\x01\\xeb\\x49\\x8b\\x34\\x8b\\x01\\xee\\x31\\xc0\\x99"
	"\\xac\\x84\\xc0\\x74\\x07\\xc1\\xca\\x0d\\x01\\xc2\\xeb\\xf4\\x3b\\x54\\x24\\x28\\x75\\xe5\\x8b\\x5f"
	"\\x24\\x01\\xeb\\x66\\x8b\\x0c\\x4b\\x8b\\x5f\\x1c\\x01\\xeb\\x03\\x2c\\x8b\\x89\\x6c\\x24\\x1c\\x61"
	"\\xc3\\x31\\xdb\\x64\\x8b\\x43\\x30\\x8b\\x40\\x0c\\x8b\\x70\\x1c\\xad\\x8b\\x40\\x08\\x5e\\x68\\x8e"
	"\\x4e\\x0e\\xec\\x50\\xff\\xd6\\x66\\x53\\x66\\x68\\x33\\x32\\x68\\x77\\x73\\x32\\x5f\\x54\\xff\\xd0"
	"\\x68\\xcb\\xed\\xfc\\x3b\\x50\\xff\\xd6\\x5f\\x89\\xe5\\x66\\x81\\xed\\x08\\x02\\x55\\x6a\\x02\\xff"
	"\\xd0\\x68\\xd9\\x09\\xf5\\xad\\x57\\xff\\xd6\\x53\\x53\\x53\\x53\\x43\\x53\\x43\\x53\\xff\\xd0\\x68"
	"(....)\\x66\\x68(..)\\x66\\x53\\x89\\xe1\\x95\\x68\\xec\\xf9\\xaa\\x60\\x57\\xff\\xd6\\x6a\\x10\\x51"
	"\\x55\\xff\\xd0\\x66\\x6a\\x64\\x66\\x68\\x63\\x6d\\x6a\\x50\\x59\\x29\\xcc\\x89\\xe7\\x6a\\x44\\x89"
	"\\xe2\\x31\\xc0\\xf3\\xaa\\x95\\x89\\xfd\\xfe\\x42\\x2d\\xfe\\x42\\x2c\\x8d\\x7a\\x38\\xab\\xab\\xab"
	"\\x68\\x72\\xfe\\xb3\\x16\\xff\\x75\\x28\\xff\\xd6\\x5b\\x57\\x52\\x51\\x51\\x51\\x6a\\x01\\x51\\x51"
	"\\x55\\x51\\xff\\xd0\\x68\\xad\\xd9\\x05\\xce\\x53\\xff\\xd6\\x6a\\xff\\xff\\x37\\xff\\xd0\\x68\\xe7"
	"\\x79\\xc6\\x79\\xff\\x75\\x04\\xff\\xd6\\xff\\x77\\xfc\\xff\\xd0\\x68\\xf0\\x8a\\x04\\x5f\\x53\\xff"
	"\\xd6\\xff\\xd0";
	mapping (host,port);
};  



connectbackfiletransfer::halle
{
	pattern
	"\\x89\\x83\\x9B\\x00\\x00\\x00\\x53\\xE8\\xEB\\x02\\x00\\x00\\x5B\\x58\\x5F\\x5E\\xE8\\x10\\x05\\x00"
	"\\x00\\xE8\\x9C\\xFE\\xFF\\xFF\\x00\\x00\\x00\\x00(....)(..)\\x77\\x73\\x32\\x5F\\x33\\x32\\x00\\x57"
	"\\x53\\x41\\x53\\x74\\x61\\x72\\x74\\x75\\x70\\x00\\x73\\x6F\\x63\\x6B\\x65\\x74\\x00";
	mapping (host,port);
/* 
 * the first 4 bytes of the transferr are the file size
 * ideas ?
 *
 */
//	flags size;

};


// taken from shellcode-generic/sch_generic_cmd.cpp
execute::cmd
{
	pattern
	".*(cmd.* /.*(\\x00|\\x0D\\x0A)).*";
	mapping (command);
};

// taken from shellcode-generic/sch_generic_createprocess.cpp
execute::createprocess
{
	pattern
	"^.*\\x0A\\x65\\x73\\x73.*\\x57\\xE8....(.*)\\x6A.\\xE8....+$";
	mapping (command);
};

// taken from shellcode-generic/sch_generic_winexec.cpp
execute::winexec
{
	pattern
	"\\xE8\\x46\\x00\\x00\\x00\\x8B\\x45\\x3C\\x8B\\x7C\\x05\\x78\\x01\\xEF\\x8B\\x4F\\x18\\x8B\\x5F\\x20"
	"\\x01\\xEB\\xE3\\x2E\\x49\\x8B\\x34\\x8B\\x01\\xEE\\x31\\xC0\\x99\\xAC\\x84\\xC0\\x74\\x07\\xC1\\xCA"
	"\\x0D\\x01\\xC2\\xEB\\xF4\\x3B\\x54\\x24\\x04\\x75\\xE3\\x8B\\x5F\\x24\\x01\\xEB\\x66\\x8B\\x0C\\x4B"
	"\\x8B\\x5F\\x1C\\x01\\xEB\\x8B\\x1C\\x8B\\x01\\xEB\\x89\\x5C\\x24\\x04\\xC3\\x31\\xC0\\x64\\x8B\\x40"
	"\\x30\\x85\\xC0\\x78\\x0F\\x8B\\x40\\x0C\\x8B\\x70\\x1C\\xAD\\x8B\\x68\\x08\\xE9\\x0B\\x00\\x00\\x00"
	"\\x8B\\x40\\x34\\x05\\x7C\\x00\\x00\\x00\\x8B\\x68\\x3C\\x5F\\x31\\xF6\\x60\\x56\\xEB\\x0D\\x68\\xEF"
	"\\xCE\\xE0\\x60\\x68\\x98\\xFE\\x8A\\x0E\\x57\\xFF\\xE7\\xE8\\xEE\\xFF\\xFF\\xFF(.*\\x00)";
	mapping (command);
};

// taken from shellcode-generic/sch_genric_wget.cpp

/*
 * curl needs other flags than wget to write to file, so ... maybe add wget & curl as VFSCommand and pass it there?
 */

download::wget
{
	pattern
	".*(wget.*)$";
	mapping(command);
};



download::curl
{
	pattern
	".*(curl.*)$";
	mapping(command);
};


// taken from shellcode-generic/sch_generic_url.cpp
url::anyurl
{
	pattern
	".*((http|https|ftp):\/\/[@a-zA-Z0-9\-\/\\\.\+:]+).*";
	mapping (uri);
};



// taken from shellcode-generic/sch_generic_link_trans.cpp
connectbacklinkfiletransfer::linktransfer
{
	pattern
	".*\\x53\\x53\\x68(....)\\x68\\x02\\x00(..)\\x8B\\xD4\\x8B\\xD8\\x6A"
//                         ^^^^->ip             ^^-> port
	"\\x10\\x52\\x53\\xBA\\x63\\x30\\x60\\x5A\\xFF\\xD6\\x50\\xB4\\x02\\x50\\x55\\x53\\xBA"
	"\\x00\\x58\\x60\\xE2\\xFF\\xD6\\xBF(....)\\xFF\\xE5.*";
//                                           ^^^^-> auth key
	mapping(host,port,key);
};


// taken from shellcode-generic/sch_generic_stuttgart.cpp
connectbacklinkfiletransfer::stuttgart
{
	pattern
	"\\x50\\x50\\x68(....)\\x68\\x02\\x00"
	"(..)\\x8B\\xFC\\x50\\x6A\\x01\\x6A\\x02\\xFF"
	"\\x55\\x20\\x8B\\xD8\\x6A\\x10\\x57\\x53\\xFF\\x55"
	"\\x24\\x85\\xC0\\x75\\x59\\xC7\\x45\\x00(....)"
	"\\x50\\x6A\\x04\\x55\\x53\\xFF\\x55\\x2C";
	mapping(host,port,key);
};


// taken from shellcode-generic/sch_generic_link_bind_trans.cpp
bindlinkfiletransfer::bindlinktransfer
{
	pattern
	"\\xba\\x83\\x53\\x83\\x00\\xff\\xd6\\x53\\x53\\x53\\x68\\x02\\x00"
	"(..)\\x8b\\xd4\\x8b\\xd8\\x6a\\x10\\x52\\x53\\xba\\x00\\x90"
	"\\xa6\\xc2\\xff\\xd6\\x40\\x50\\x53\\xba\\x7a\\x3b\\x73\\xa1\\xff"
	"\\xd6\\x50\\x50\\x53\\xba\\x10\\xd3\\x69\\x00\\xff\\xd6\\x8b\\xd8"
	"\\x33\\xc0\\x50\\xb4\\x02\\x50\\x55\\x53\\xba\\x00\\x58\\x60\\xe2"
	"\\xff\\xd6\\xbf(....)\\xff\\xe5";

	mapping (port,key);
};


