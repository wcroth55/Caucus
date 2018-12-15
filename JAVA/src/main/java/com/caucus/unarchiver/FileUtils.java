package com.caucus.unarchiver;

/**
* OLAT - Online Learning and Training<br>
* http://www.olat.org
* <p>
* Licensed under the Apache License, Version 2.0 (the "License"); <br>
* you may not use this file except in compliance with the License.<br>
* You may obtain a copy of the License at
* <p>
* http://www.apache.org/licenses/LICENSE-2.0
* <p>
* Unless required by applicable law or agreed to in writing,<br>
* software distributed under the License is distributed on an "AS IS" BASIS, <br>
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. <br>
* See the License for the specific language governing permissions and <br>
* limitations under the License.
* <p>
* Copyright (c) 1999-2006 at Multimedia- & E-Learning Services (MELS),<br>
* University of Zurich, Switzerland.
* <p>
*/ 

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Iterator;

//import org.olat.core.logging.AssertException;
//import org.olat.core.logging.OLATRuntimeException;

/**
 * @author Mike Stock Comment:
 */
public class FileUtils {

	private static int buffSize = 32 * 1024;

	/**
	 * @param sourceFile
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean copyFileToDir(String sourceFile, String targetDir) {
		return copyFileToDir(new File(sourceFile), new File(targetDir), false);
	}

	/**
	 * @param sourceFile
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean moveFileToDir(String sourceFile, String targetDir) {
		return copyFileToDir(new File(sourceFile), new File(targetDir), true);
	}

	/**
	 * @param sourceFile
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean copyFileToDir(File sourceFile, File targetDir) {
		return copyFileToDir(sourceFile, targetDir, false);
	}

	/**
	 * @param sourceFile
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean moveFileToDir(File sourceFile, File targetDir) {
		return copyFileToDir(sourceFile, targetDir, true);
	}

	/**
	 * @param sourceDir
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean copyDirToDir(String sourceDir, String targetDir) {
		return copyDirToDir(new File(sourceDir), new File(targetDir), false);
	}

	/**
	 * @param sourceDir
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean moveDirToDir(String sourceDir, String targetDir) {
		return copyDirToDir(new File(sourceDir), new File(targetDir), true);
	}

	/**
	 * @param sourceDir
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean copyDirToDir(File sourceDir, File targetDir) {
		return copyDirToDir(sourceDir, targetDir, false);
	}

	/**
	 * @param sourceDir
	 * @param targetDir
	 * @return true upon success
	 */
	public static boolean moveDirToDir(File sourceDir, File targetDir) {
		return copyDirToDir(sourceDir, targetDir, true);
	}

	/**
	 * Get the size in bytes of a directory
	 * 
	 * @param path
	 * @return true upon success
	 */
	public static long getDirSize(File path) {
		Iterator<File> path_iterator;
		File current_file;
		long size;

		File[] f = path.listFiles();
		if (f == null) { return 0; }
		path_iterator = (Arrays.asList(f)).iterator();
		size = 0;
		while (path_iterator.hasNext()) {
			current_file = (File) path_iterator.next();
			if (current_file.isFile()) {
				size += current_file.length();
			} else {
				size += getDirSize(current_file);
			}
		}
		return size;
	}

	/**
	 * Copy the contents of a directory from one spot on hard disk to another. Will create any
	 * target dirs if necessary.
	 * 
	 * @param sourceDir directory which contents to copy on local hard disk.
	 * @param targetDir new directory to be created on local hard disk.
	 * @param move
	 * @return true if the copy was successful.
	 */
	public static boolean copyDirContentsToDir(File sourceDir, File targetDir, boolean move) {
		return copyDirInternal(sourceDir, targetDir, move, false);
	}
	
	/**
	 * Copy a directory from one spot on hard disk to another. Will create any
	 * target dirs if necessary. The directory itself will be created on the target location.
	 * 
	 * @param sourceDir directory to copy on local hard disk.
	 * @param targetDir new directory to be created on local hard disk.
	 * @param move
	 * @return true if the copy was successful.
	 */
	public static boolean copyDirToDir(File sourceDir, File targetDir, boolean move) {
		return copyDirInternal(sourceDir, targetDir, move, true);
	}

	/**
	 * Copy a directory from one spot on hard disk to another. Will create any
	 * target dirs if necessary.
	 * 
	 * @param sourceDir directory to copy on local hard disk.
	 * @param targetDir new directory to be created on local hard disk.
	 * @param move
	 * @param createDir If true, a directory with the name of the source directory will be created
	 * @return true if the copy was successful.
	 */
	private static boolean copyDirInternal(File sourceDir, File targetDir, boolean move, boolean createDir) {

		if (sourceDir.isFile()) return copyFileToDir(sourceDir, targetDir, move);
		if (!sourceDir.isDirectory()) return false;

		targetDir.mkdirs(); // this will also copy/move empty directories
		if (!targetDir.isDirectory()) return false;

		if (createDir) targetDir = new File(targetDir, sourceDir.getName());
		targetDir.mkdirs();
		boolean success = true;
		String[] fileList
		= sourceDir.list();
		if (fileList == null) return false; // I/O error or not a directory
		for (int i = 0; i < fileList.length; i++) {
			File f = new File(sourceDir, fileList[i]);
			if (f.isDirectory()) success = success && copyDirToDir(f, targetDir, move);
			else {
				success = success && copyFileToDir(f, targetDir, move);
			}
		}
		return success;
	}

	/**
	 * Copy a file from one spot on hard disk to another. Will create any target
	 * dirs if necessary.
	 * 
	 * @param sourceFile file to copy on local hard disk.
	 * @param targetDir new file to be created on local hard disk.
	 * @param move
	 * @return true if the copy was successful.
	 */
	public static boolean copyFileToDir(File sourceFile, File targetDir, boolean move) {
		try {
			// catch if source is directory by accident
			if (sourceFile.isDirectory()) { return copyDirToDir(sourceFile, targetDir, move); }

			// create target directories
			targetDir.mkdirs(); // don't check for success... would return false on
			// existing dirs
			if (!targetDir.isDirectory()) return false;
			File targetFile = new File(targetDir, sourceFile.getName());

			// catch move/copy of "same" file -> buggy under Windows.
			if (sourceFile.getCanonicalPath().equals(targetFile.getCanonicalPath())) return true;
			if (move) { return sourceFile.renameTo(targetFile); }

			// C O P Y S O U R C E T O T A R G E T
			FileInputStream is = new FileInputStream(sourceFile);
			FileOutputStream os = new FileOutputStream(targetFile);
			long fileLength = sourceFile.length();
			boolean success = copy(is, os, fileLength);
			if (!success) { return false; }
			is.close();
			os.close();
		} catch (IOException e) {
			return false;
		}
		return true;
	} // end copy

	/**
	 * Copy an InputStream to an OutputStream.
	 * 
	 * @param source InputStream, left open.
	 * @param target OutputStream, left open.
	 * @param length how many bytes to copy.
	 * @return true if the copy was successful.
	 */
	public static boolean copy(InputStream source, OutputStream target, long length) {
		if (length == 0) return true;
		try {
			int chunkSize = (int) Math.min(buffSize, length);
			long chunks = length / chunkSize;
			int lastChunkSize = (int) (length % chunkSize);
			// code will work even when chunkSize = 0 or chunks = 0;
			byte[] ba = new byte[chunkSize];

			for (long i = 0; i < chunks; i++) {
				int bytesRead = readBlocking(source, ba, 0, chunkSize);
				if (bytesRead != chunkSize) { throw new IOException(); }
				target.write(ba);
			} // end for
			// R E A D / W R I T E last chunk, if any
			if (lastChunkSize > 0) {
				int bytesRead = readBlocking(source, ba, 0, lastChunkSize);
				if (bytesRead != lastChunkSize) { throw new IOException(); }
				target.write(ba, 0, lastChunkSize);
			} // end if
		} catch (IOException e) {
			return false;
		}
		return true;
	} // end copy

	/**
	 * Copy an InputStream to an OutputStream, until EOF. Use only when you don't
	 * know the length.
	 * 
	 * @param source InputStream, left open.
	 * @param target OutputStream, left open.
	 * @return true if the copy was successful.
	 */
	public static boolean copy(InputStream source, OutputStream target) {
		try {

			int chunkSize = buffSize;
			// code will work even when chunkSize = 0 or chunks = 0;
			// Even for small files, we allocate a big buffer, since we
			// don't know the size ahead of time.
			byte[] ba = new byte[chunkSize];

			while (true) {
				int bytesRead = readBlocking(source, ba, 0, chunkSize);
				if (bytesRead > 0) {
					target.write(ba, 0, bytesRead);
				} else {
					break;
				} // hit eof
			} // end while
		} catch (IOException e) {
			return false;
		}
		return true;
	} // end copy

	/**
	 * Reads exactly <code>len</code> bytes from the input stream into the byte
	 * array. This method reads repeatedly from the underlying stream until all
	 * the bytes are read. InputStream.read is often documented to block like
	 * this, but in actuality it does not always do so, and returns early with
	 * just a few bytes. readBlockiyng blocks until all the bytes are read, the
	 * end of the stream is detected, or an exception is thrown. You will always
	 * get as many bytes as you asked for unless you get an eof or other
	 * exception. Unlike readFully, you find out how many bytes you did get.
	 * 
	 * @param in
	 * @param b the buffer into which the data is read.
	 * @param off the start offset of the data.
	 * @param len the number of bytes to read.
	 * @return number of bytes actually read.
	 * @exception IOException if an I/O error occurs.
	 */
	public static final int readBlocking(InputStream in, byte b[], int off, int len) throws IOException {
		int totalBytesRead = 0;

		while (totalBytesRead < len) {
			int bytesRead = in.read(b, off + totalBytesRead, len - totalBytesRead);
			if (bytesRead < 0) {
				break;
			}
			totalBytesRead += bytesRead;
		}
		return totalBytesRead;
	} // end readBlocking

	/**
	 * Get rid of ALL files and subdirectories in given directory, and all subdirs
	 * under it,
	 * 
	 * @param dir would normally be an existing directory, can be a file aswell
	 * @param recursive true if you want subdirs deleted as well
	 * @param deleteDir true if dir needs to be deleted as well
	 * @return true upon success
	 */
	public static boolean deleteDirsAndFiles(File dir, boolean recursive, boolean deleteDir) {

		boolean success = true;

		if (dir == null) return false;

		// We must empty child subdirs contents before can get rid of immediate
		// child subdirs
		if (recursive) {
			String[] allDirs = dir.list();
			if (allDirs != null) {
				for (int i = 0; i < allDirs.length; i++) {
					success &= deleteDirsAndFiles(new File(dir, allDirs[i]), true, false);
				}
			}
		}

		// delete all files in this dir
		String[] allFiles = dir.list();
		if (allFiles != null) {
			for (int i = 0; i < allFiles.length; i++) {
				File deleteFile = new File(dir, allFiles[i]);
				success &= deleteFile.delete();
			}
		}

		// delete passed dir
		if (deleteDir) {
			success &= dir.delete();
		}
		return success;
	} // end deleteDirContents

	/**
	 * @param newF
	 */
	public static void createEmptyFile(File newF) {
		try {
			FileOutputStream fos = new FileOutputStream(newF);
			fos.close();
		} catch (IOException e) {
			throw new AssertException("empty file could not be created for path " + newF.getAbsolutePath(), e);
		}

	}
	
	
	/**
	 * @param baseDir
	 * @param fileVisitor
	 */
	public static void visitRecursively(File baseDir, FileVisitor fileVisitor) {
		visit(baseDir, fileVisitor);
	}
	
	private static void visit(File file, FileVisitor fileVisitor) {
		if (file.isDirectory()) {
			File[] files = file.listFiles();
			for (int i = 0; i < files.length; i++) {
				File f = files[i];
				visit(f, fileVisitor);
			}
		}
		else { // regular file
			fileVisitor.visit(file);
		}
	}
	
	
	/**
	 * @param target
	 * @param data
	 * @param encoding
	 */
	public static void save(File target, String data, String encoding) {
		try {
			save(new FileOutputStream(target), data, encoding);
		} catch (IOException e) {
			throw new OLATRuntimeException(FileUtils.class, "could not save file", e);
		}
	}

	/**
	 * @param target
	 * @param data
	 * @param encoding
	 */
	public static void save(OutputStream target, String data, String encoding) {
		try {
			byte[] ba = data.getBytes(encoding);
			ByteArrayInputStream bis = new ByteArrayInputStream(ba);
			boolean success = FileUtils.copy(bis, target);
			bis.close();
			target.close();
			if (!success) throw new OLATRuntimeException(FileUtils.class, "could not save to output stream",null);
		} catch (IOException e) {
			throw new OLATRuntimeException(FileUtils.class, "could not save to output stream", e);
		}
	}

	/**
	 * @param source
	 * @param encoding
	 * @return the file in form of a string
	 */
	public static String load(File source, String encoding) {
		try {
			return load(new FileInputStream(source), encoding);
		} catch (FileNotFoundException e) {
			throw new RuntimeException("could not copy file to ram: " + source.getAbsolutePath());
		}
	}

	/**
	 * @param source
	 * @param encoding
	 * @return the inpustream in form of a string
	 */
	public static String load(InputStream source, String encoding) {
		String htmltext = null;
		try {
			ByteArrayOutputStream bas = new ByteArrayOutputStream();
			boolean success = FileUtils.copy(source, bas);
			source.close();
			bas.close();
			if (!success) throw new RuntimeException("could not copy inputstream to ram");
			htmltext = bas.toString(encoding);
		} catch (IOException e) {
			throw new OLATRuntimeException(FileUtils.class, "could not load from inputstream", e);
		}
		return htmltext;
	}

	/**
	 * @param is
	 */
	public static void closeSafely(InputStream is) {
		try {
			is.close();
		} catch (IOException e) {
			// nothing to do
		}
		
	}
	
	/**
	 * @param os
	 */
	public static void closeSafely(OutputStream os) {
		try {
			os.close();
		} catch (IOException e) {
			// nothing to do
		}
		
	}	

	/**
	 * Extract file suffix. E.g. 'html' from index.html
	 * @param filePath
	 * @return return empty String "" without suffix. 
	 */
	public static String getFileSuffix(String filePath) {
		int lastDot = filePath.lastIndexOf('.');
		if (lastDot > 0) {
			if (lastDot < filePath.length())
				return filePath.substring(lastDot + 1).toLowerCase();
		}
		return "";
	}

}
