
int cond_destroy(tCondVariable* cond)
{
	if(!cond)
		return -1;

	kinterface->kcondition_destroy(cond);

	return 0;
}

