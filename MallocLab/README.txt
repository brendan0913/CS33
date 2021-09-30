My implementation takes much of the textbook code and only slightly 
adapts the preexisting functions (except for the heavily modified find fit
function). Additionally, very simple functions to maintain the segregated list
are added (add, delete, set_seg_index, select_seg_list, set_seg_list). The last
three functions are akin to getters and setters of the seg list pointers.
 * Segregated free list: 10 buckets (10 global pointers)
 * Best-fit: Search from lower bucket to higher buckets, choose the best fit among 
 * the first 3 fits, or return best fit so far when reaching a higher bucket.
 
 My solution was in the top 10% in performance among the class, and scored extra credit.
 
 Grade: 127/100
