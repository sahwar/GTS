#include <fstream>
#include "osapi_exist.h"
#include "osapi_getenv.h"	/* osapi::getenv(-) */
#include "osapi_mkdir.h"
#include "gts_file_path.h"

#define PATH_SEPARETER	('/')

namespace {

bool exist_file_(const char *filename) {
	/*FILE  *fp;
	if ((fp = ::fopen(filename, "r")) == NULL) {
		return false;
	}
	::fclose(fp);
	return true;*/

	std::ifstream ifs( filename );
	if (ifs) { return true; }
	return false;
}
void file_path_from_dir_(std::string& dir_path, const char *file_name) {
	if (dir_path.empty()) {
		return;
	}

	dir_path += PATH_SEPARETER;
	dir_path += file_name;
	if (!exist_file_(dir_path.c_str())) {
		dir_path = std::string();
	}
}
std::string get_dexe_home_(const char *comm) {
	std::string path(comm);
	std::string::size_type index = path.find_last_of("/\\");
	if (std::string::npos != index) {
		path.erase(index);
	}
	return path;
}

/* 環境変数を得る */
/*void ptbl_getenv_(const char *name, std::string& env) {
	char *value = ptbl_getenv(name);
	if (value != nullptr) {
		env = value;
		free(value);
	}
}*/

} // namespace

//---------------------------------------------------------------

#ifndef _WIN32
const char* get_desktop_dir_when_unix( void )
{
	return ".GTS";
}
#endif

void ptbl_get_user_home(std::string& user_home)
{
#ifdef _WIN32
	std::string homedrive( osapi::getenv( "HOMEDRIVE" ) );
	std::string homepath(  osapi::getenv( "HOMEPATH"  ) );
	if (!homedrive.empty() && !homepath.empty()) {
		user_home = homedrive;
		user_home += homepath;
	}
#else
	user_home = osapi::getenv( "HOME" );
#endif
}

std::string gts_file_path(const char *comm, const char *file_name) {
	/* 優先度A  各ユーザーのホームにあるなら
	--> %HOMEDRIVE%%HOMEPATH%\_gts-scan_area.txt"
	=         "C:\Users\user1\_gts-scan_area.txt"
	*/
	std::string fpath_user;
	ptbl_get_user_home(fpath_user);
#ifndef _WIN32
	fpath_user += PATH_SEPARETER;
	fpath_user += get_desktop_dir_when_unix();
	if ( osapi::exist( fpath_user ) == false ) {
		if ( osapi::mkdir( fpath_user ) == false ) {
			/* フォルダ作れないなら、親フォルダを返す */
			ptbl_get_user_home(fpath_user);
			return fpath_user;
		}
	}
#endif
	file_path_from_dir_(fpath_user, file_name);
	if (!fpath_user.empty()) {
		return fpath_user;
	}

#ifdef PKGDATADIR
	std::string fpath_data(PKGDATADIR);
	file_path_from_dir_(fpath_data, file_name);
	if (!fpath_data.empty()) {
		return fpath_data;
	}
#endif

	/* 優先度B  全ユーザープロファイルのホームにあるなら
 	--> %ALLUSERSPROFILE%\_gts-scan_area.txt"
	= "C:\Users\All Users\_gts-scan_area.txt" at WindowsXp 
	=     "C:\ProgramData\_gts-scan_area.txt" at Windows7  
		Windows7では一般ユーザーが書き込めないので使えない
	*/
	std::string fpath_prof( osapi::getenv( "ALLUSERSPROFILE" ) );

	file_path_from_dir_(fpath_prof, file_name);
	if (!fpath_prof.empty()) {
		return fpath_prof;
	}

	/* 優先度C  共有のホームにあるなら
	-->	%PUBLIC%\_gts-scan_area.txt"
	= "C:\Users\Public\_gts-scan_area.txt" at Windows7
	*/
	std::string fpath_publ( osapi::getenv( "PUBLIC" ) );

	file_path_from_dir_(fpath_publ, file_name);
	if (!fpath_publ.empty()) {
		return fpath_publ;
	}

	/* 優先度D  .exeと同じ場所にあるなら */
	std::string fpath_dexe( get_dexe_home_(comm) );

	file_path_from_dir_(fpath_dexe, file_name);
	if (!fpath_dexe.empty()) {
		return fpath_dexe;
	}

	/* A,B,C,Dどのファイルもないならなにもしない */
	return std::string();
}
