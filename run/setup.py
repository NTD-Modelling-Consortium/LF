import setuptools

setuptools.setup(
    name='ntd_lf_combiner',
    version='0.0.1',
    url='https://www.ntdmodelling.org',
    maintainer='ArtRabbit',
    maintainer_email='support@artrabbit.com',
    description='LF simulation model output combiner',
    packages=setuptools.find_packages(),
    python_requires='>=3.9',
    install_requires=[
		'pandas'
    ],
    include_package_data=True
)
