#include <stdio.h>
#include <stdlib.h>

typedef struct reg{
    int n, distance;
    double dt; 
    int* seq;
} reg;
typedef struct HD{
    int m, head;
    double size_mm, dw, msper_track; 
    int* rings;
} HD;
typedef struct CPU{
    int seq_size, num_of_scheduling_algorithms;
    HD* hd;
    reg *ui_buffer, *fifo, *sstf, *scan, *cscan;
    double* ratios;
} CPU;

void print_menu(int uiloop, HD* hd){
    if(uiloop == 1)
        printf("\nAssumptions:\n\n\t1. 5.25-inch Hard Disk or (133.35 mm).\n\t2. The width of each track = %f mm.\n\t3. Arm-speed of %f ms/track.\n\n", hd->dw, hd->msper_track);
    printf("\n\tDisk Scheduling Menu:\n----------------------------------------\n1. Enter parameters.\n2. Calculate distance to traverse tracks using FIFO.\n3. Calculate distance to traverse tracks using SSTF.\n4. Calculate distance to traverse tracks using Scan.\n5. Calculate distance to traverse tracks using C-Scan.\n6. Quit program and free memory.\n\nYour Selection: ---> ");
}
void sds(){
    printf("\n\n/////////////////////////////////////////////////////////////////////\n\n");
}
int get_int(){
    int i = 0;
    scanf("%d", &i);
    return i;
}
int get_ui(HD* hd, int uiloop){
    int ui = 0;
    const int MIN = 1, MAX = 6;
    while((ui < MIN) || (ui > MAX)){
        sds();
        print_menu(uiloop, hd);
        ui = get_int(ui);
    }
    return ui;
}
CPU* userinput_sequence_size(CPU* cpu){
    while(1){
        printf("\nEnter the size of the sequence: ");
        cpu->seq_size = get_int(cpu->seq_size);
        if((cpu->seq_size >= 0) && (cpu->seq_size < cpu->hd->m))
            break;
        printf("\nHard disk contains %d rings. Enter a value less than %d.\n", cpu->hd->m, cpu->hd->m);
    }
    return cpu;
}
CPU* userinput_startingtrack(CPU* cpu){
    while(1){
        printf("\n\nEnter starting track: "); 
        cpu->ui_buffer->seq[0] = get_int();
        if(!(cpu->ui_buffer->seq[0] >= cpu->hd->m) && !(cpu->ui_buffer->seq[0] < 1))
            break;
        printf("\nStarting track # '%d' is invalid because hard drive only contains %d tracks (1 - %d).\n", cpu->ui_buffer->seq[0], cpu->hd->m, cpu->hd->m);
    }
    return cpu;
}
CPU* userinput_to_uibuffer_sequence(CPU* cpu){
    int seq_entry = 1;
    printf("\n");
    while(seq_entry < cpu->seq_size){
        while(1){
            printf("\nEnter the sequence value that follows the previous track in the queue: previous(%d)--><next>: next = ", cpu->ui_buffer->seq[(seq_entry - 1)]);
            cpu->ui_buffer->seq[seq_entry] = get_int();
            if(!(cpu->ui_buffer->seq[seq_entry] >= cpu->hd->m) && !(cpu->ui_buffer->seq[seq_entry] < 1))
                break;
            printf("\nsequence value: %d is larger then the # of tracks '%d' contained in hard drive, re-enter a # to replace '%d'.\n", cpu->ui_buffer->seq[seq_entry], cpu->hd->m, cpu->ui_buffer->seq[seq_entry]);
        }
        seq_entry++;
    }
    return cpu;
}
CPU* initiate_cpu(CPU* cpu){
    cpu = (CPU*)(malloc(sizeof(HD*) + (5 * sizeof(reg*)) + (2 * sizeof(int)) + sizeof(double*)));
    cpu->hd = (HD*)malloc(sizeof(int*) + (2 * sizeof(int)) + (3 * sizeof(double)) );
    cpu->num_of_scheduling_algorithms = 5;
    cpu->hd->msper_track = 1.375;
    cpu->hd->head = 0;
    cpu->hd->rings = (int*)malloc(sizeof(int) * cpu->hd->m);
    cpu->hd->dw = .160;
    cpu->hd->size_mm = 133.35;
    cpu->seq_size = -1;
    cpu->hd->m = (int)((double)(cpu->hd->size_mm / cpu->hd->dw));
    cpu->ui_buffer = (reg*)malloc( (2 * sizeof(int)) + (sizeof(int*)) + sizeof(double));
    cpu->fifo = (reg*)malloc( (2 * sizeof(int)) + (sizeof(int*)) + sizeof(double));
    cpu->sstf = (reg*)malloc( (2 * sizeof(int)) + (sizeof(int*)) + sizeof(double));
    cpu->scan = (reg*)malloc( (2 * sizeof(int)) + (sizeof(int*)) + sizeof(double));
    cpu->cscan = (reg*)malloc( (2 * sizeof(int)) + (sizeof(int*)) + sizeof(double));
    return cpu;
}
CPU* malloc_registers(CPU* cpu){
    cpu->ui_buffer->n = cpu->seq_size;
    cpu->fifo->n = cpu->seq_size;
    cpu->sstf->n = cpu->seq_size;
    cpu->scan->n = cpu->seq_size;
    cpu->cscan->n = cpu->seq_size;
    cpu->ui_buffer->seq = (int*)malloc(sizeof(int) * cpu->seq_size);
    cpu->fifo->seq = (int*)malloc(sizeof(int) * cpu->seq_size);
    cpu->sstf->seq = (int*)malloc(sizeof(int) * cpu->seq_size);
    cpu->scan->seq = (int*)malloc(sizeof(int) * cpu->seq_size);
    cpu->cscan->seq = (int*)malloc(sizeof(int) * cpu->seq_size);
    cpu->ratios = (double*)malloc(sizeof(double) * cpu->num_of_scheduling_algorithms);
    return cpu;
}
reg* copy_registers(reg* orig, reg* copy){
    for(int i = 0; i < orig->n; i++){
        copy->seq[i] = orig->seq[i];
    }
    return copy;
}
void print_register(reg* r, char* title){
    printf("%s", title);
    for(int i = 0; i < r->n; i++){
        if(i == (r->n - 1)){
            printf(" %d.", r->seq[i]);
            break;
        }
        printf(" %d,", r->seq[i]);
    }
}
CPU* fill_hd_tracks(CPU* cpu){
    for(int i = 0; i < cpu->seq_size; i++){
        cpu->hd->rings[cpu->ui_buffer->seq[i]] = 1;
    }
    return cpu;
}
CPU* calculate_traversal_inorder(CPU* cpu, reg* r){
    int last = -1, current;
    for(int i = 0; i < cpu->seq_size; i++){
        if(last == -1){
            current = r->seq[i];
            r->distance = r->seq[i];
        }
        else {
            current = r->seq[i];
            if(current <= last){
                r->distance += last - current;
            }
            else{
                r->distance += current - last;
            }
        }
        last = current;
    }
    return cpu;
}
void print_traversal_report(char* title, reg* r, int* s, HD* hd, int uiloop){
    if(uiloop < 60000){
        printf("\n\n\t\t%s\n\n", title);
        //
        printf("\n\tA. Original sequence: ");
        for(int j = 0; j < r->n; j++){
             printf(" %d", s[j]);
        }
        printf("\n\tB. Traversed sequence: ");
        for(int i = 0; i < r->n; i++){
             printf(" %d", r->seq[i]);
        }
        printf("\n\tC. The distance traveled by hard drive head: %d tracks.", r->distance);
        r->dt = (double)(r->distance * hd->msper_track);
        printf("\n\tD. %f milliseconds/track * %d tracks-travelled = %f milli-seconds total for %s\n", hd->msper_track, r->distance, r->dt, title);
    }
    else{
        printf("\n\t%s", title);
        printf("\n\tThe distance traveled by hard drive head: %d tracks.", r->distance);
        printf("\n\t%f milliseconds/track * %d tracks-travelled = %f milli-seconds total.\n", hd->msper_track, r->distance, r->dt);
    }
}
int get_index_of(reg* r, int p, int gap){
    for(int i = p+1; i < r->n; i++){
        if((r->seq[p] - r->seq[i] == gap) || (r->seq[p] - r->seq[i] == (-1 * gap))){
            return i;
        }
    }
    return -1;
}
reg* run_sstf(CPU* cpu, reg* r, HD* hd){
    int* arr = (int*)malloc(sizeof(int) * r->n);
    for(int i = 0; i < r->n; i++){
        arr[i] = cpu->ui_buffer->seq[i];
    }
    int curr = hd->m, smallest = hd->m, small_i = -1, p = 1, temp = 0;
    for(int i = 0; i < r->n - 1; i++){
        for(int j = i+1; j < r->n; j++){
            while(r->seq[j] == -1){ 
                j += 1;
            }
            if(j == r->n){
                break;
            }
            curr = r->seq[i] - r->seq[j];
            if(curr < 0){
                curr = ((-1) * (curr));
            }
            
            if(curr < smallest){
                smallest = curr;
                small_i = get_index_of(r, i, smallest);
                
            }
        }
        temp = r->seq[small_i];
        r->seq[small_i] = r->seq[i+1];
        r->seq[i+1] = temp;
        smallest = hd->m;
    }
    return r;
}
reg* run_inc_scan(reg* scan, int* seq){
    //done
    int smallest = 10000, small_i, curr, temp = 0, orig = seq[0];
    for(int i = 0; i < scan->n - 1; i++){
        for(int j = i+1; j < scan->n; j++){
            if(seq[j] - seq[i] > 0){
                curr = seq[j] - seq[i];
                if(curr < smallest){
                    smallest = curr;
                    small_i = j;
                }
            }
        }
        temp = seq[i+1];
        seq[i+1] = seq[small_i];
        seq[small_i] = temp;
        smallest = 100000;
    }
    curr = 0;
    while(seq[curr+1] > seq[curr]){
        curr++;
    }
    if(curr == scan->n - 1)
        return scan;
    curr++;
    for(int i = curr; i < scan->n -1; i++){
        for(int j = i + 1; j < scan->n; j++){
            if(seq[i] < seq[j]){
                temp = seq[j];
                seq[j] = seq[i];
                seq[i] = temp;
            }
        }
    }
    scan->seq = seq;
    return scan;
}
reg* run_dec_scan(reg* scan, int* seq){
    int smallest = 10000, small_i = 0, curr, temp = 0, orig = seq[0];
    for(int i = 0; i < scan->n - 1; i++){
        for(int j = i+1; j < scan->n; j++){
            if(seq[j] - seq[i] < 0){
                curr = -1 * (seq[j] - seq[i]);
                if(curr < smallest){
                    smallest = curr;
                    small_i = j;
                }
            }
        }
        temp = seq[i+1];
        seq[i+1] = seq[small_i];
        seq[small_i] = temp;
        smallest = 100000;
    }
    curr = 0;
    while(seq[curr+1] < seq[curr]){
        curr++;
    }
    if(curr == scan->n - 1)
        return scan;
    curr++;
    for(int i = curr; i < scan->n -1; i++){
        for(int j = i + 1; j < scan->n; j++){
            if(seq[i] > seq[j]){
                temp = seq[j];
                seq[j] = seq[i];
                seq[i] = temp;
            }
        }
    }
    scan->seq = seq;
    return scan;
}
int* get_cscan_order(int* seq, int seq_size, int m, reg* cscan){
    int curr = seq[0], smallest = m, small_i = 1, temp, index = 0;
    for(int i = 0; i < seq_size-1; i++){
        for(int j = i+1; j < seq_size; j++){
            curr = seq[j] - seq[i];
            if(curr > 0){
                if(curr < smallest){
                    smallest = curr;
                    small_i = j;
                }
            }
        }
        temp = seq[i+1];
        seq[i+1] = seq[small_i];
        seq[small_i] = temp;
        smallest = m;
        index++;
    }
    
    return seq;
}
int index_of_first_smaller(int* s){
    int c = 0;
    while(1){
       if(s[c+1] < s[c]){
           c++;
           break;
       }
       c++;
    }
    return c;
}
int* finish_ordering(reg* cscan, int* s, int new_i){
    int temp = 0;
    for(int i = new_i; i < cscan->n - 1; i++){
        for(int j = i + 1; j < cscan->n; j++){
            if(s[j] < s[i]){
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
            }
        }
    }
    return s;
}
int isfastestratio(double fastest, double curr, double* r, int n){
    for(int i = 0; i < n; i ++){
        if(r[i] == 0){
            
        }
    }
}
//1 20 284 5 62 85 685 103 53 82 465 278 95 796 2 77 709 627 9 564 2 509 2 3 4 0 4 1 5 6  ---> ONLINE GDB TEST SET 
int main() {
    printf("\n\n\n\n\tAssignment #5 Disk Scheduling:\n\nCreated By: Brandon Teller.\nClass: Comp-322 CSUN Spring-2022.\n");
    CPU* cpu;
    int ui = 0, entered_p = 0, scan = -1, index = 0, ratios_index, isfaster = 0, uiloop = 1, fastestindex = -1;
    double  fastest = -1, curr;
    const int MIN = 1, MAX = 6;
    int* io;
    cpu = initiate_cpu(cpu);
    while(!(ui == 6)){
        while(1){
            ui = get_ui(cpu->hd, uiloop);
            if(entered_p == 1)
                break;
            if((entered_p == 0) && (ui == 1)){
                entered_p = 1;
                break;
            }
        }
        switch(ui){
            case(1):
                sds();
                printf("\n\tEntering Parameters:\n\n");
                cpu = userinput_sequence_size(cpu);
                cpu = malloc_registers(cpu);
                cpu = userinput_startingtrack(cpu);
                cpu = userinput_to_uibuffer_sequence(cpu);
                sds();
                cpu->hd->head = cpu->ui_buffer->seq[0];
                break;
            case(2):
                sds();
                ratios_index = ui - 2;
                cpu->fifo = copy_registers(cpu->ui_buffer, cpu->fifo);
                cpu = calculate_traversal_inorder(cpu, cpu->fifo);
                print_traversal_report("FIFO Traversal Report.", cpu->fifo, cpu->fifo->seq, cpu->hd, uiloop);
                cpu->ratios[ratios_index] = (double)((double)cpu->fifo->distance / (double)cpu->fifo->distance);
                curr = cpu->ratios[ratios_index];
                if((uiloop == 2) && (ui != 6)){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                if(curr < fastest){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                sds();
                break; 
            case(3):
                sds();
                ratios_index = ui - 2;
                cpu->sstf = copy_registers(cpu->ui_buffer, cpu->sstf);
                cpu->sstf = run_sstf(cpu, cpu->sstf, cpu->hd);
                cpu = calculate_traversal_inorder(cpu, cpu->sstf);
                print_traversal_report("SSTF Traversal Report.", cpu->sstf, cpu->fifo->seq, cpu->hd, uiloop);
                cpu->ratios[ratios_index] = (double)((double)cpu->sstf->distance / (double)cpu->fifo->distance);
                curr = cpu->ratios[ratios_index];
                if((uiloop == 2) && (ui != 6)){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                if(curr < fastest){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                sds();
                break;
            case(4):
                sds();
                printf("\n\tScan Disk-Scheduling:\n");
                while(1){
                    printf("\nEnter '0' for decreasing, or '1' for increasing: ");
                    scanf("%d", &scan);
                    if((scan == 1) || (scan == 0))
                        break;
                    printf("\nPlease make a valid choice '0' or '1'.\n");
                }
                ratios_index = ui - 2 + scan;
                cpu->scan = copy_registers(cpu->ui_buffer, cpu->scan);
                if(scan == 0)
                    cpu->scan = run_dec_scan(cpu->scan, cpu->scan->seq);
                else{
                    cpu->scan = run_inc_scan(cpu->scan, cpu->scan->seq);
                }
                cpu = calculate_traversal_inorder(cpu, cpu->scan);
                print_traversal_report("SCAN Traversal Report.", cpu->scan, cpu->fifo->seq, cpu->hd, uiloop);
                cpu->ratios[ratios_index] = (double)((double)cpu->scan->distance / (double)cpu->fifo->distance);
                curr = cpu->ratios[ratios_index];
                if((uiloop == 2) && (ui != 6)){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                if(curr < fastest){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                sds();
                break;
            case(5):
                sds();
                ratios_index = ui - 1;
                cpu->cscan = copy_registers(cpu->ui_buffer, cpu->cscan);
                cpu->cscan->seq = get_cscan_order(cpu->cscan->seq, cpu->cscan->n, cpu->hd->m, cpu->cscan);
                index = index_of_first_smaller(cpu->cscan->seq);
                cpu->cscan->seq = finish_ordering(cpu->cscan, cpu->cscan->seq, index);
                cpu = calculate_traversal_inorder(cpu, cpu->cscan);
                print_traversal_report("CSCAN Traversal Report.", cpu->cscan, cpu->fifo->seq, cpu->hd, uiloop);
                cpu->ratios[ratios_index] = (double)((double)cpu->cscan->distance / (double)cpu->fifo->distance);
                curr = cpu->ratios[ratios_index];
                if((uiloop == 2) && (ui != 6)){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                if(curr < fastest){
                    fastest = curr;
                    fastestindex = ratios_index;
                }
                sds();
                break;
            case(6):
            uiloop = 60000;
                sds();
                printf("\n\n\t\t***Effiency Report***\n\n");
                if(!(fastestindex == -1)){
                    printf("Original order of sequence from Userinput Buffer (FIFO):");
                    print_register(cpu->fifo, "\n\tOriginal FIFO:");
                    printf("\n\tThis represents %d percent of Userinput Buffer's Original Sequence.", (int)(100 * cpu->ratios[0]));
                     print_traversal_report("FIFO Time Report:", cpu->fifo, cpu->fifo->seq, cpu->hd, uiloop);
                    printf("\n\nFastest Path Observed, from YOUR SELECTIONS, was the ");
                    switch(fastestindex){
                        case(0):
                            printf("FIFO Algorithm which cut path by ~%d percent.\n", (int)(100 * (1 - cpu->ratios[0])));
                            print_register(cpu->fifo, "\tFastest Path:");
                            print_traversal_report("FIFO Time Report:", cpu->fifo, cpu->fifo->seq, cpu->hd, uiloop);
                            break;
                        case(1):
                            printf("SSTF Algorithm which cut path by ~%d percent.\n", (int)(100 * (1 - cpu->ratios[1])));
                            print_register(cpu->sstf, "\tFastest Path:");
                            print_traversal_report("SSTF Time Report:", cpu->sstf, cpu->fifo->seq, cpu->hd, uiloop);
                            break;
                        case(2):
                            printf("Decreasing SCAN Algorithm which cut path by ~%d percent.\n", (int)(100 * (1 - cpu->ratios[2])));
                            print_register(cpu->scan, "\tFastest Path:");
                            print_traversal_report("SCAN Time Report:", cpu->scan, cpu->fifo->seq, cpu->hd, uiloop);
                            break;
                        case(3):
                            printf("Increasing SCAN Algorithm which cut path by ~%d percent.\n", (int)(100 * (1 - cpu->ratios[3])));
                            print_register(cpu->scan, "\tFastest Path:");
                            print_traversal_report("SCAN Time Report:", cpu->scan, cpu->fifo->seq, cpu->hd, uiloop);
                            break;
                        case(4):
                            printf("CSCAN Algorithm which cut path by ~%d percent.\n", (int)(100 * (1 - cpu->ratios[4])));
                            print_register(cpu->cscan, "\tFastest Path:");
                             print_traversal_report("CSCAN Time Report.", cpu->cscan, cpu->fifo->seq, cpu->hd, uiloop);
                            break;
                        default:
                            break;
                    }
                }
                printf("\n\n\n\n\tExiting Program....\n\n");
                free(cpu);
                printf("\nMemory Free!\n");
                sds();
                return 1;
            default:
                break;
        }
        ui = 0;
        uiloop++;
    }
    return 0; 
}
