
Advanced Preferences
=========

With the advanced parameters in JASP you can specify the following options:
(All these settings will remain after restarting JASP.)

## Modules options

### Remember enabled modules
If you've enabled this option then JASP will remember which modules are activated and make sure they remain that way even when you close JASP. So supposing `Summary Statistics` was enabled and JASP closes then after reopening JASP it will be enabled immediately.

The CRAN repository URL determines where JASP will try to download the required packages specified in a module from.
The default is `https://cloud.r-project.org` but a good alternative (when packages seemingly can't be installed for instance) is `cran.r-project.org`.


### Github personal access tokens
When a dynamic module is installed JASP uses R internally to get all dependencies and for this it requests info from https://github.com and often this goes well.
They do have a rate limiter there however, see https://docs.github.com/en/rest/overview/resources-in-the-rest-api#rate-limiting and that can cause the module installation to fail.

To work around this we have added some options to manage a so called personal access token, and it is then passed on to R through an environment variable called `GITHUB_PAT`.
We added a default one, but this is shared with all JASP users and thus might (at some point) become oversaturated. In that case you might want to add your own.

This can be done by generating a new token for your github account at https://github.com/settings/tokens/new and then either copying the resulting code into the settings here in JASP or through your OS. If set in your OS, you might have done this for R already, JASP will automatically use it if set use "the default" value.

Please *DO NOT* give this personal access token **any permissions** at all, because it really isn't necessary for the way it is used in JASP.

Otherwise you can set it specifically in JASP by unchecking "Use default PAT" and then copying your token-code into the custom GITHUB_PAT textbox, this is probably the easiest if you do not know what an environment variable is.

### Developer mode
This is where you specify if you want to use JASP development modules or not (see section *Development module* below).


## Development module

The development module option allows you to enter a libpath and use that to load modules from.  This mean you can build/install a module with its dependencies in Rstudio and then load it into JASP.
For example load a module project and run:
```
renv::activate()
renv::restore()
renv::install('.')
.libPaths()
```

Copy the first entry into the libpath menu textbox along with the name of the module you wish to load. After that, open the sidebar with the modules and select "Install Developer Module" to install your locally built module. Each time you adjust something in your module, such as the .R or .qml files, rebuild the module in Rstudio and refresh the developer module in JASP. You can also use keyboard shortcuts: `ctrl+shift+R` for refreshing the R part, `ctrl+shift+U` for the qml part, and `ctrl+shift+D` for refreshing the whole module.

## Logging options

### Log to file
When you check this JASP will start logging many of the actions it performs to logfiles. 
Logging is especially useful when you are developing your own module, or run into a problem and wish to reach out to the development team.
The logs might help us give insight in the nature of your problem. Note that you might need to restart JASP for the logging process to start.
The number in the input field "Max logfiles to keep" defines how many logfiles will be kept at maximum to conserve diskspace. Any extra, older, logfiles will be removed.
The files can be viewed by pressing "Show logs".
