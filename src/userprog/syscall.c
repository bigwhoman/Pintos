#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "lib/kernel/console.h"
#include "filesys/filesys.h"
// #include "filesys/file.h"
// #include "lib/kernel/list.h"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

static void syscall_handler (struct intr_frame *);

//maybe we do not need it but lets see
struct file_item {
  int fd;
  void* file;
  struct list_elem elem;
};

struct list all_files;
int fd_num;

void
syscall_init (void)
{
  fd_num = 2;
  list_init(&all_files);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  uint32_t* args = ((uint32_t*) f->esp);

  /*
   * The following print statement, if uncommented, will print out the syscall
   * number whenever a process enters a system call. You might find it useful
   * when debugging. It will cause tests to fail, however, so you should not
   * include it in your final submission.
   */

  //  printf("System call number: %d\n", args[0]); 

  if (args[0] == SYS_EXIT)
    {
      f->eax = args[1];
      printf ("%s: exit(%d)\n", &thread_current ()->name, args[1]);
      thread_exit ();
    }
  else if (args[0] == SYS_PRACTICE) {
    f -> eax = args[1] + 1;
  }
  else if (args[0] == SYS_HALT) {
      shutdown_power_off();
  }
  else if (args[0] == SYS_WRITE) {
      int fd = args[1];
      char * buffer = (char *)(void *)args[2];
      uint32_t size = args[3];
      if (fd == STDOUT_FILENO) {
        putbuf(buffer, size);
      } else if (fd == STDIN_FILENO){
        // pass 
      } else {

      }
  }
  else if (args[0] == SYS_OPEN) {
    char *file_name = (char *)(void *)args[1];
    if (file_name == NULL){
	    f -> eax = -1;
	    return;
    }
    struct file* file_to_open = filesys_open(file_name);
    int fd = -1;
    if(file_to_open == NULL){
	
    } else {
    	struct file_item *new_file = malloc(sizeof(struct file_item));
   	  new_file -> fd = fd_num;
    	new_file -> file = file_to_open;
    	list_push_back(&all_files, &new_file->elem);
      fd = fd_num;
      fd_num ++;
    }
    f -> eax = fd;
  }
}


/* 
  
 Quick thing for myself : use this for list itteration ( probably would be needed )

    struct file_item *new_file2 = malloc(sizeof(struct file_item));
    new_file2 -> fd = 31;
    new_file2 -> file = file_to_open;
    list_push_back(&all_files, &new_file2->elem);

    struct list_elem *e;
    for (e = list_begin(&all_files); e != list_end(&all_files); e = list_next(e)) {
       struct file_item *f = list_entry(e, struct file_item, elem);
       printf("%d ----- %p \n", f->fd, f->file);
    }
*/
