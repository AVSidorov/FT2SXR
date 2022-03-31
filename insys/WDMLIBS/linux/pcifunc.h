
#include "../drvlibs/dzycross.h"

u32 pci_resource_start( struct pci_dev *dev, int bar );
u32 pci_resource_end( struct pci_dev *dev, int bar );
u32 pci_resource_flags( struct pci_dev *dev, int bar );
int pci_request_regions( struct pci_dev *dev, char *devname );
int pci_release_regions( struct pci_dev *dev );

void *pci_alloc_consistent( struct pci_dev *dev, 
			    size_t size, 
			    dma_addr_t *bus_addr );
void  pci_free_consistent( struct pci_dev *dev, 
			    size_t size, 
			    void *cpuaddr,
			    dma_handle_t bus_addr );
			    
inline int get_order(unsigned long size);