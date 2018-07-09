import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class Make {
	private static final String objectFilesDir = "objects";
	private static final String srcFilesDir = "src";
	private static final String cCompileCommand = "i686-elf-gcc -Isrc/include/ "+
			"-Isrc/include/libc/ -include \"stddef.h\" -c \"%s\" -o \"" + objectFilesDir 
			+ "\\%s.o\" -std=gnu99 -ffreestanding -O2 -Wall -Wextra";
	private static final String asmCompileCommand = "nasm -f elf32 \"%s\" -o \"" + objectFilesDir + 
			"\\%s.o\"";
	
	private static final String linkCommand = "i686-elf-gcc -T linker.ld -o FemtoOS.bin -ffreestanding -O2 -nostdlib %s -lgcc";
	
	private static ProcessBuilder pb;
	
	private static final int COMPILE = 1, LINK = 1 << 1, CREATE_ISO = 1 << 2, RUN = 1 << 3;
	
	public static void main(String[] args) throws Exception {
		pb = new ProcessBuilder();
		pb.inheritIO();

		int flags = 0;
		
		if(args.length >= 1) {
			if(args[0].contains("compile")) {
				flags |= COMPILE;				
			}
			
			if(args[0].contains("link")) {
				flags |= LINK;				
			}
			
			if(args[0].contains("iso")) {
				flags |= CREATE_ISO;				
			}
			
			if(args[0].contains("run")) {
				flags |= RUN;				
			}
		} else {
			flags = 0xffff;
		}
		
		if((flags & COMPILE) != 0) {
			System.out.println("Compilation:\n");
			
			if(makeCompile()) {
				System.out.println("Problems with compilation.");
				return;
			}
		}
		
		if((flags & LINK) != 0) {
			System.out.println("Linking:\n");
			
			if(makeLink()) {
				System.out.println("Problems with linking.");
				return;
			}
		}
		
		if((flags & CREATE_ISO) != 0) {
			System.out.println("ISO creating:\n");
			
			try {
				Files.delete(Paths.get("isofiles/boot/FemtoOS.bin"));
			} catch(Exception e) {System.out.println(e.getLocalizedMessage());}
			
			try {
				Files.copy(Paths.get("FemtoOS.bin"), Paths.get("isofiles/boot/FemtoOS.bin"));
			} catch(Exception e) {System.out.println(e.getLocalizedMessage());}
			
			if(executeCommand("mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o FemtoOS.iso isofiles/")) {
				System.out.println("Problems with iso creating.");
				return;
			}
		}
		
		if((flags & RUN) != 0) {
			System.out.println("Launching:\n");
			
			if(executeCommand("qemu-system-x86_64 -boot d -cdrom FemtoOS.iso -m 128")) {
				System.out.println("Problems with running.");
				
				return;
			}
		}
		
	}

	public static boolean makeCompile() throws Exception {
		ArrayList<File> cFiles = findFiles(srcFilesDir, new String[] {"c", "asm"});
		ArrayList<File> objectFiles = findFiles(objectFilesDir, new String[] {"o"});
		
		for(File cFile: cFiles) {
	        String fileName = cFile.getName();
	        
			int pos = fileName.lastIndexOf(".");

	        if(pos > 0) {
	        	fileName = fileName.substring(0, pos);
	        }
	        
	        boolean compile = true;
	        
	        Iterator<File> oi = objectFiles.iterator();
	        
	        while(oi.hasNext()) {
	        	File oFile = oi.next();
	        	
	        	if(oFile.getName().contains(fileName)) {
					oi.remove();
					
					if(oFile.lastModified() >= cFile.lastModified()) {
						compile = false;
						
						break;
					}
				}
	        }
	        
	        if(compile) {
	        	if(cFile.getName().endsWith("asm")) {
	        		if(executeCommand(String.format(asmCompileCommand, cFile.getPath(), fileName))) {
	        			return true;
		        	}
		        } else {
		        	if(executeCommand(String.format(cCompileCommand, cFile.getPath(), fileName))) {
	        			return true;
		        	}
		        }
	        }
		}
		
		for(File obFile: objectFiles) {
			obFile.delete();
		}
		
		return false;
	}
	
	public static boolean makeLink() throws Exception {
		ArrayList<File> objectFiles = findFiles(objectFilesDir, new String[] {"o"});
		
		StringBuilder b = new StringBuilder();
		
		for(File f: objectFiles) {
			b.append('"').append(f.getPath()).append("\" ");
		}
		
		return executeCommand(String.format(linkCommand, b.toString()));
	}
	
	public static boolean executeCommand(String command) throws Exception {
		List<String> arg = new ArrayList<>();
		
		int lastPos = 0;
		
		for(int i = 0; i < command.length(); ++i) {
			if(command.charAt(i) == '"') {
				while((i + 1) < command.length() && command.charAt(++i) != '"');
			}
			
			if(command.charAt(i) == ' ') {
				arg.add(command.substring(lastPos, i));
				lastPos = i + 1;
			}
		}
		
		arg.add(command.substring(lastPos, command.length()));
		
		pb.command(arg);
		return pb.start().waitFor() == 0 ? false : true;
	}

	public static ArrayList<File> findFiles(String path, String[] extensions) {
		ArrayList<File> ret = new ArrayList<>();
		
		_findFiles(path, extensions, ret);
		
		return ret;
	}
	
	private static void _findFiles(String path, String[] extensions, ArrayList<File> files) {
		try {
			File file = new File(path);
			
			if(file.isDirectory()) {
				String[] fln = file.list();
		    	
				for(int i = 0; i < fln.length; ++i){
		            _findFiles(path + "/" + fln[i], extensions, files);
		    	}
			
			} else {				
				int index = path.lastIndexOf('.');
				
				if (index > 0) {
				    String extension = path.substring(index + 1);
				    
				    for(int i = 0; i < extensions.length; ++i) {
				    	if(extension.equalsIgnoreCase(extensions[i])) {
				    		files.add(file);
				    	}
				    }
				}
			}
		} catch(Exception e) {}
	}
}
