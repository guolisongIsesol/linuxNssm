# linuxNssm
a small tool used to register service in linux just like nssm in windows

//nssm install serviceName filePath [role], when role is empty,means role is root(in such case 4 parameters)
	nssm install demo /home/gls/demo   :demo is the serviceName, demo's whole path is /home/gls/demo, run in root role
	nssm install demo /home/gls/demo gls : the gls means demo runs in the role of gls
//nssm remove serviceName
	nssm remove demo  : to delete the demo 
//nssm start servicename
	nssm start demo : to start the service demo
//nssm stop serviceName
	nssm stop demo : to stop demo
//nssm status serviceName
	nssm status demo : check demo's status, return -1, means demo is not a service. return 0 means demo is a service but stopped, return 1 means demo is a service and is running
//nssm restart serviceName:
	nssm restart demo: restart the service demo