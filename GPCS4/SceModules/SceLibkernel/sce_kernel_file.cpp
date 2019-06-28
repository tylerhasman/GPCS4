#include "sce_libkernel.h"
#include "sce_kernel_file.h"
#include "Platform/UtilPath.h"
#include <io.h>

#ifdef GPCS4_WINDOWS

#include "dirent/dirent.h"




// since windows doesn't allow open(directory),
// we record both DIR* and fd in this slot array
ulong_ptr g_fdSlots[SCE_FD_MAX] = { 0 };

inline uint getEmptySlotIdx()
{
	uint idx = 0;
	// note, we shouldn't use index 0, it's an invalid fd
	for (uint i = 1; i != SCE_FD_MAX; ++i)
	{
		if (g_fdSlots[i] != NULL)
		{
			continue;
		}
		idx = i;
		break;
	}

	if (idx == 0)
	{
		LOG_ERR("exceed max fd count.");
	}

	return idx;
}

inline bool isDirFd(uint idx)
{
	bool isDir = false;
	do 
	{
		if (idx >= SCE_FD_MAX)
		{
			LOG_ERR("exceed max fd count.");
			break;
		}

		// for dir, we store DIR* in slots
		// and for x64 program, pointer value > 0xFFFFFFFF
		if (g_fdSlots[idx] > 0xFFFFFFFF)
		{
			isDir = true;
			break;
		}

	} while (false);
	return isDir;
}

inline bool getDirName(DIR* dir, char* dirname, int len)
{
	bool bRet = false;
	do
	{
		wchar_t* wfname = dir->wdirp->patt;
		if (!wfname)
		{
			break;
		}

		if (wcstombs(dirname, wfname, len) == (size_t)-1)
		{
			break;
		}
		
		int str_len = strlen(dirname);
		dirname[str_len - 2] = 0;
		dirname[str_len - 1] = 0;
		
		bRet  = true;
	}while(false);
	return bRet;
}

#endif  //GPCS4_WINDOWS

int PS4API sceKernelOpen(const char *path, int flags, SceKernelMode mode)
{
	LOG_SCE_TRACE("path %s flag %x mode %x", path, flags, mode);
	std::string pcPath = UtilPath::PS4PathToPCPath(path);
#ifdef GPCS4_WINDOWS
	uint idx = getEmptySlotIdx();
	if (flags & SCE_KERNEL_O_DIRECTORY)
	{
		DIR* dir = opendir(pcPath.c_str());
		g_fdSlots[idx] = (ulong_ptr)dir;
	}
	else
	{
		
	}

	return idx;
#endif  //GPCS4_WINDOWS
}


ssize_t PS4API sceKernelRead(int d, void *buf, size_t nbytes)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


ssize_t PS4API sceKernelWrite(int d, const void *buf, size_t nbytes)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelLseek(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelClose(int d)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelFstat(int fd, SceKernelStat *sb)
{
	LOG_SCE_TRACE("fd %d sb %p", fd, sb);

#ifdef GPCS4_WINDOWS
	int ret = -1;
	bool isDir = isDirFd((uint)fd);
	if (isDir)
	{
		char dir_path[SCE_MAX_PATH] = { 0 };
		getDirName((DIR*)g_fdSlots[fd], dir_path, SCE_MAX_PATH);
		struct _stat stat;
		ret = _stat(dir_path, &stat);
		sb->st_mode = stat.st_mode;
		//sb->st_atim = stat.st_atime;
		//sb->st_mtim = stat.st_mtime;
		//sb->st_ctim = stat.st_ctime;
		sb->st_size = stat.st_size;
		//sb->st_birthtim = stat.st_ctime; //?
		sb->st_blocks = UtilPath::FileCountInDirectory(dir_path);
		sb->st_blksize = sizeof(SceKernelDirent);
	}
	else
	{

	}

	return ret;
#endif  //GPCS4_WINDOWS
}


int PS4API sceKernelFsync(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelFtruncate(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

inline byte getSceFileType(dirent* ent)
{
	byte type = SCE_KERNEL_DT_UNKNOWN;
	if ( (ent->d_namlen == 1 && !strncmp(ent->d_name, ".", 1)) ||
		 (ent->d_namlen == 2 && !strncmp(ent->d_name, ".", 2)) )
	{
		type = SCE_KERNEL_DT_DIR;
	}
	else
	{
		switch (ent->d_type)
		{
		case DT_DIR:
			type = SCE_KERNEL_DT_DIR;
			break;
		case DT_REG:
			type = SCE_KERNEL_DT_REG;
			break;
		default:
			LOG_ERR("found unknown file type. file %s type %x", ent->d_name, ent->d_type);
			break;
		}
	}

	return type;
}

int PS4API sceKernelGetdents(int fd, char *buf, int nbytes)
{
	LOG_SCE_TRACE("fd %d buff %p nbytes %x", fd, buf, nbytes);
#ifdef GPCS4_WINDOWS
	int ret = SCE_KERNEL_ERROR_EBADF;
	do 
	{
		bool isDir = isDirFd((uint)fd);
		if (!isDir)
		{
			ret = SCE_KERNEL_ERROR_EINVAL;
			break;
		}

		DIR* dir = (DIR*)g_fdSlots[fd];
		dirent *ent;
		ent = readdir(dir);
		if (!dir)
		{
			ret = 0;  //ends
			break;
		}

		SceKernelDirent* sce_ent = (SceKernelDirent*)buf;
		sce_ent->d_fileno = ent->d_ino;
		sce_ent->d_reclen = sizeof(SceKernelDirent);
		sce_ent->d_type = getSceFileType(ent);
		sce_ent->d_namlen = ent->d_namlen;
		strncpy(sce_ent->d_name, ent->d_name, ent->d_namlen);

		ret = sizeof(SceKernelDirent);
	} while (false);

	return ret;

#endif  //GPCS4_WINDOWS
}


int PS4API sceKernelMkdir(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelRename(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelStat(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}


int PS4API sceKernelUnlink(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}
