var bwt = require("./bwt.js");
var fs = require("fs")
var fiber = require('fibers');


var infileName = process.argv[2];
var outfileName = process.argv[3];
var blockSize = parseInt(process.argv[4]);
 
console.dir("Using blocksize " + blockSize);
 
bwtOnFile(blockSize, infileName, outfileName);

function getFilesizeInBytes(filename) {
 var stats = fs.statSync(filename)
 var fileSizeInBytes = stats["size"]
 return fileSizeInBytes
}
 
function bwtOnFile(blocksize, infile, outfile) { 
	var alreadyProcessed = 0;
	var filesize = getFilesizeInBytes();
	//Open the file to write to
	var targetFD = fs.openSync(outfile, 'w');
	//Open a fd
	fs.open(infile, 'r', 0666, function(err, fd) {
		if(err) {console.dir(err);}
		//Read blocksize bytes
		while(1) {
			var buf = new Buffer(blocksize);
			var bytesRead = fs.readSync(fd, buf, 0, blocksize, null); //Read blocksize bytes
			//Calculate the BWT
			var bwtBuf = bwt.bwtFromBuf(buf);
			fs.write(targetFD, buf, 0, buf.length, null, function(err, written, buffer) {
				if(err) {console.dir(err);}
				alreadyProcessed += blocksize;
				if(written != buf.length) {
					console.dir("Written only " + written + " of  " + buf.length + " bytes");
				}
		});
	}
	fs.closeSync(fd);
	console.dir("Finished");
	});
	fs.closeSync(targetFD);
}
